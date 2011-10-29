#include "os_stdio.h"
#include "net_e100.h"
#include "string.h"
#include "os_memory.h"
#include "memory.h"

static struct pci_data_st e100 = {0};

struct nic nic;

static int e100_exec_cmd(int csr_comp, uint8_t cmd);

static void e100_init();

static void cbl_init();
static void cbl_alloc();
static void cbl_validate ();
static int cbl_append_nop (uint16_t flag);
static int cbl_append_transmit (const char *data, uint16_t l, uint16_t flag);

static void rfa_init();
static void rfa_alloc();
static void rfa_validate ();
static int rfa_retrieve_data (char *data);
/*
* 0x84 is an unused port,
* time to execute the inb is appromimately 1.25us
* delay about 10us.
*/
static
__inline void delay(int i)
{
	while(--i >=0) {
		inb(0x84);
	}
}

/* OSE lab6: E100 Reset */
static void e100_reset()
{
	outl(e100.addr_base[E100_IO] + CSR_PORT, PORT_SW_RESET);
	delay(10);
}

/**
 * execute a cmd via e100
 * clear the interrupt, load base for CU and RU and so on
 */
static int
e100_exec_cmd (int csr_comp, uint8_t cmd)
{
    int retry = 30;
    int scb_command;

    outb(nic.io_base + csr_comp, cmd);
    do {
        scb_command = inb(nic.io_base + CSR_COMMAND);
    } while (scb_command != 0 && --retry > 0);
    return retry > 0;
}

/* TBM: update with mit ose */
#define page2kva(page) (page)

struct Page {
	physaddr_t addr;
	uint16_t pp_ref;
};
#define HAVE_PAGE_NET 1
#define MAX_PAGE_FOR_NET 64
static struct Page *page_net;
static uint32_t page_index;

static int page_net_init()
{
	page_net = (struct Page *) memman_alloc_4k((struct MEMMAN *) MEMMAN_ADDR, PGSIZE);
	memset(page_net, 0, PGSIZE);
	page_index = 0;
	return 0;
}

static __inline 
physaddr_t page2pa(struct Page *pp)
{
	return pp->addr;
}

static int page_alloc(struct Page **pp)
{
#if HAVE_PAGE_NET
	struct Page *p;
	if (page_index >= MAX_PAGE_FOR_NET)
		return 1;

	p = &page_net[page_index++];
	p->addr = (physaddr_t) memman_alloc_4k((struct MEMMAN *) MEMMAN_ADDR, PGSIZE);
	p->pp_ref = 0;
	*pp = p;
#endif
	return 0;
}

/**
 * Allocate CB_MAX_NUM pages, each page for a control block
 */
static void
cbl_alloc () {
    int i, r;
    struct Page *p;
    struct cb *prevcb = NULL;
    struct cb *currcb = NULL;

    // Allocate physical page for Control block
    for (i = 0; i < CB_MAX_NUM; i++) {

        if ((r = page_alloc (&p)) != 0)
            panic ("cbl_init: run out of physical memory! %e\n", r);

        p -> pp_ref ++;
        memset (page2kva (p), 0, PGSIZE);

        currcb = (struct cb *)page2kva (p);
        currcb->phy_addr = page2pa (p);


        if (i == 0)
            nic.cbl.start = currcb;
        else {
            prevcb->cb_link = currcb->phy_addr;
            prevcb->next = currcb;
            currcb->prev = prevcb;
        }

        prevcb = currcb;
    }

    prevcb->cb_link = nic.cbl.start->phy_addr;
    nic.cbl.start->prev = prevcb;
    prevcb->next = nic.cbl.start;

    nic.cbl.cb_avail = CB_MAX_NUM;
    nic.cbl.cb_wait = 0;

    nic.cbl.front = nic.cbl.start;
    nic.cbl.rear = nic.cbl.start->prev;
}

static int
cbl_append_nop (uint16_t flag)
{
    if (nic.cbl.cb_avail == 0)
        return -E_CBL_FULL;

    nic.cbl.cb_avail --;
    nic.cbl.cb_wait ++;

    nic.cbl.rear = nic.cbl.rear->next;

    nic.cbl.rear->cb_status = 0;
    nic.cbl.rear->cb_control = CBC_NOP | flag;

    return 0;
}

static int
cbl_append_transmit (const char *data, uint16_t l, uint16_t flag)
{
    if (nic.cbl.cb_avail == 0)
        return -E_CBL_FULL;

    nic.cbl.cb_avail --;
    nic.cbl.cb_wait ++;

    nic.cbl.rear = nic.cbl.rear->next;

    nic.cbl.rear->cb_status = 0;
    nic.cbl.rear->cb_control = CBC_TRANSMIT | flag;

    nic.cbl.rear->cb_cmd_spec.tcb.tcb_tbd_array_addr    = 0xFFFFFFFF;
    nic.cbl.rear->cb_cmd_spec.tcb.tcb_byte_count        = l;
    nic.cbl.rear->cb_cmd_spec.tcb.tcb_thrs              = 0xE0;
    nic.cbl.rear->cb_cmd_spec.tcb.tcb_tbd_count         = 0;

    memmove (nic.cbl.rear->cb_cmd_spec.tcb.tcb_data, (void *)data, l);

    return 0;
}

int e100_transmit (const char *data, uint16_t len)
{
    cbl_validate ();

    if (nic.cbl.cb_avail == 0)
        return -E_CBL_FULL;
    
    nic.cbl.rear->cb_control &= ~CBF_S;
    cbl_append_transmit (data, len, CBF_S);

    int scb_status = inb(nic.io_base + CSR_STATUS);
    if ((scb_status & CUS_MASK) == CUS_SUSPENDED)
        e100_exec_cmd (CSR_COMMAND, CUC_RESUME); 

    return 0;
}

typedef int (*func_net_rx)(char *);
typedef int (*func_net_tx)(const char *, uint16_t);

static func_net_rx _nic_rx = &e100_receive;
static func_net_tx _nic_tx = &e100_transmit;

int net_receive(char *data)
{
	return _nic_rx(data);
}
int net_transmit (const char *data, uint16_t len)
{
	return _nic_tx(data, len);
}

static void
cbl_validate () 
{
    while (nic.cbl.cb_wait > 0 && (nic.cbl.front->cb_status & CBS_C) != 0) {
        nic.cbl.front = nic.cbl.front->next;
        nic.cbl.cb_avail ++;
        nic.cbl.cb_wait --;
    }
}

static void
cbl_init () 
{
    cbl_alloc ();

    cbl_append_nop (CBF_S);

    outl(nic.io_base + CSR_GP, nic.cbl.front->phy_addr);
    e100_exec_cmd (CSR_COMMAND, CUC_START); 
}



/**
 * Allocate RFD_MAX_NUM pages each page for a Recieve Frame Descriptor
 */
static void
rfa_alloc () {
    int i, r;
    struct Page *p;
    struct rfd *prevrfd = NULL;
    struct rfd *currrfd = NULL;

    // Allocate physical page for Control block
    for (i = 0; i < RFD_MAX_NUM; i++) {
        if ((r = page_alloc (&p)) != 0)
            panic ("rfa_init: run out of physical memory! %e\n", r);

        p -> pp_ref ++;
        memset (page2kva (p), 0, PGSIZE);

        currrfd = (struct rfd *)page2kva (p);
        currrfd->phy_addr = page2pa (p);
        currrfd->rfd_control = 0;
        currrfd->rfd_status = 0;
        currrfd->rfd_size = RFD_MAXSIZE;

        if (i == 0)
            nic.rfa.start = currrfd;
        else {
            prevrfd->rfd_link = currrfd->phy_addr;
            prevrfd->next = currrfd;
            currrfd->prev = prevrfd;
        }

        prevrfd = currrfd;
    }

    prevrfd->rfd_link = nic.rfa.start->phy_addr;
    nic.rfa.start->prev = prevrfd;
    prevrfd->next = nic.rfa.start;

    nic.rfa.rfd_avail = RFD_MAX_NUM;
    nic.rfa.rfd_wait = 0;

    nic.rfa.front = nic.rfa.start;
    nic.rfa.rear = nic.rfa.start->prev;
    nic.rfa.rear->rfd_control |= RFDF_S;
}


static void
rfa_init () 
{
    //cprintf ("\n\nRFA Initialization started! \n");

    rfa_alloc ();

    outl(nic.io_base + CSR_GP, nic.rfa.front->phy_addr);
    e100_exec_cmd (CSR_COMMAND, RUC_START); 


/**
 * This section is for test when we finished rfa_alloc ()  
 **/
//#define HAVE_RFA_ALLOC_TEST
#ifdef HAVE_RFA_ALLOC_TEST
    while (nic.rfa.rfd_avail > 0)
        rfa_validate ();

    int scb_status = inb(nic.io_base + CSR_STATUS);

    printf ("rfd slot is full, current RU state = %02x\n", scb_status & RUS_MASK);

    char s[1518];
    while (rfa_retrieve_data (s) >= 0);


    e100_exec_cmd (CSR_COMMAND, RUC_RESUME);

    while (nic.rfa.rfd_avail > 0)
        rfa_validate ();
#endif /* test rfa_allc */
}


static void
rfa_validate () 
{
    while (nic.rfa.rfd_avail > 0 && (nic.rfa.rear->next->rfd_status & RFDS_C) != 0) {
        nic.rfa.rear = nic.rfa.rear->next;

        nic.rfa.rfd_avail --;
        nic.rfa.rfd_wait ++;
        //cprintf ("validate, avail = %d, wait = %d,   slot = %x\n", 
        //    nic.rfa.rfd_avail, nic.rfa.rfd_wait, nic.rfa.rear);
    }
}

static int
rfa_retrieve_data (char* data)
{
    if (nic.rfa.rfd_wait == 0)
        return -E_RFA_EMPTY;

    nic.rfa.rfd_avail ++;
    nic.rfa.rfd_wait --;
    //cprintf ("retrieve, avail = %d, wait = %d,   slot = %x\n", 
        //nic.rfa.rfd_avail, nic.rfa.rfd_wait, nic.rfa.front);

    nic.rfa.front->prev->rfd_control &= ~RFDF_S;
    nic.rfa.front->rfd_control = RFDF_S;
    nic.rfa.front->rfd_status = 0;

    int r = nic.rfa.front->rfd_actual_count & RFD_AC_MASK;
    memmove (data, nic.rfa.front->rfd_data, r);

    nic.rfa.front = nic.rfa.front->next;

    return r;
}

int 
e100_receive (char *data)
{
    rfa_validate ();

    if (nic.rfa.rfd_wait == 0)
        return -E_RFA_EMPTY;

    int r = rfa_retrieve_data (data);

    int scb_status = inb(nic.io_base + CSR_STATUS);
    if ((scb_status & RUS_MASK) == RUS_SUSPEND)
        e100_exec_cmd (CSR_COMMAND, RUC_RESUME);

    return r;
}

static void e100_init()
{
	int r = 0;
	page_net_init();
	/* reset device before using */
	e100_reset();
	printf("e100_reset\n");

	/* all interrupts to be disabled */
	r = e100_exec_cmd(CSR_INT, 1);
	/* printf("e100 CSR_INT ret=%d\n", r); */

	cbl_init();
	rfa_init();
	/* printf("rfa_init, page_index=%d\n", page_index); */
}

int e100_attach(pci_pdata_t pd) {
	pci_func_enable(pd);
	e100.busno = pd->busno;
	e100.slot = pd->slot;
	e100.func = pd->func;
	e100.device = pd->device;
	e100.vendor = pd->vendor;
	e100.class_code= pd->class_code;
	int i;
	for (i = 0; i < 6; i++) {
		e100.addr_base[i] = pd->addr_base[i];
		e100.addr_size[i] = pd->addr_size[i];
	}
	e100.irq_line = pd->irq_line;

	// initialize network interface controller
	nic.io_base = pd->addr_base[E100_IO];
	nic.io_size = pd->addr_size[E100_IO];

	// initize e100 driver
	e100_init();

	return 0;
}

#if 0
int nic_detail(void)
{
	printf("eth0 Link encap:Ethernet  HWaddr 52:54:00:12:34:56 "
               "     inet addr:10.0.2.15 Bcast:10.0.2.255  Mask:255.255.255.0"
               "     UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1"
               "     RX packets:5 errors:0 dropped:0 overruns:0 frame:0"
               "     TX packets:6 errors:0 dropped:0 overruns:0 carrier:0"
               "     collisions:0 txqueuelen:100 "
               "     RX bytes:2004 (1.9 KiB)  TX bytes:1746 (1.7 KiB)"
               "     Interrupt:9 Base address:0xc100\n");
	return 1;
}
#endif 

int nic_e100(void)
{
	show_pci_enabled(&e100);
	show_reg_info(0, e100.addr_size[0], e100.addr_base[0]);
	show_reg_info(1, nic.io_size, nic.io_base);
    	printf ("cbl avail = %d, wait = %d, slot = %p, irq=%d\n",
        	 nic.cbl.cb_avail, nic.cbl.cb_wait, nic.cbl.front,
		 e100.irq_line);
    	printf ("rfa avail = %d, wait = %d, slot = %p\n", 
        	 nic.rfa.rfd_avail, nic.rfa.rfd_wait, nic.rfa.front);
	return 5; /* screen line for print */
}

int show_nic(void)
{
	return nic_e100();
}
