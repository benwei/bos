#ifndef NET_E100_H
#define NET_E100_H

#include "os_pci.h"

#define E100_VENDOR 0x8086
// #define E100_DEVICE 0x100E
#define E100_DEVICE 0x1209

// CSR Address Registers
#define E100_MEMORY     0
#define E100_IO         1
#define E100_FLASH      2


// SCB component offset in CSR
#define CSR_SCB         0x0
#define CSR_STATUS      0x0
#define CSR_US          0x0
#define CSR_STATACK     0x1
#define CSR_COMMAND     0x2
#define CSR_UC          0x2
#define CSR_INT         0x3
#define CSR_GP          0x4
#define CSR_PORT        0x8


// PORT Interface
#define PORT_SW_RESET   0x0
#define PORT_SELF_TEST  0x1
#define PORT_SEL_RESET  0x2


// CU Status Word
#define CUS_MASK        0xc0
#define CUS_IDLE        0x00
#define CUS_SUSPENDED   0x40
#define CUS_LPQ_ACTIVE  0x80
#define CUS_HQP_ACTIVE  0xc0


// CU Command Word
#define CUC_NOP         0x00
#define CUC_START       0x10
#define CUC_RESUME      0x20
#define CUC_LD_COUNTER  0x40
#define CUC_DUMP_SCNT   0x50
#define CUC_LOAD_BASE   0x60
#define CUC_DUMP_RSCNT  0x70
#define CUC_SRESUME     0xa0



// RU Status Word
#define RUS_MASK        0x3c
#define RUS_IDLE        0x0
#define RUS_SUSPEND     0x4
#define RUS_NORES       0x8
#define RUS_READY       0x10



// RU Command Word
#define RUC_NOP         0x0
#define RUC_START       0x1
#define RUC_RESUME      0x2
#define RUC_REDIR       0x3
#define RUC_ABORT       0x4
#define RUC_LOADHDS     0x5
#define RUC_LOAD_BASE   0x6



// Control Block Command
#define CBF_EL          0x8000
#define CBF_S           0x4000
#define CBF_I           0x2000

#define CBC_NOP         0x0
#define CBC_IAS         0x1
#define CBC_CONFIG      0x2
#define CBC_MAS         0x3
#define CBC_TRANSMIT    0x4
#define CBC_LOADMC      0x5
#define CBC_DUMP        0x6
#define CBC_DIAGNOSE    0x7


// Control Block Status
#define CBS_F           0x0800
#define CBS_OK          0x2000
#define CBS_C           0x8000




// Recieve Frame Descriptor Command
#define RFDF_EL         0x8000
#define RFDF_S          0x4000
#define RFDF_H          0x10
#define RFDF_SF         0x8


// Recieve Frame Descriptor Status
#define RFDS_C          0x8000
#define RFDS_OK         0x2000
#define RFDS_MASK       0x1fff


// Recieve Frame Descriptor Data
#define RFD_SIZE_MASK   0x3fff
#define RFD_AC_MASK     0x3fff
#define RFD_EOF         0x8000
#define RFD_F           0x4000




// Error CODE
#define E_CBL_FULL    1
#define E_CBL_EMPTY   2
#define E_RFA_FULL    3
#define E_RFA_EMPTY   4


#define TCB_MAXSIZE     1518
#define RFD_MAXSIZE     1518
#define CB_MAX_NUM      10
#define RFD_MAX_NUM     10


// Transmit Command Blocks
struct tcb {
    uint32_t tcb_tbd_array_addr;
    uint16_t tcb_byte_count;
    uint8_t tcb_thrs;
    uint8_t tcb_tbd_count;
    char tcb_data[TCB_MAXSIZE];
};
int e100_transmit (const char *data, uint16_t len);

// Control Blocks
struct cb {
    volatile uint16_t cb_status;
    uint16_t cb_control;
    uint32_t cb_link;

    union cb_cmd_spec_data {
        struct tcb tcb;
    } cb_cmd_spec;


    struct cb *prev, *next;
    physaddr_t phy_addr;   
};



// Control Block List
struct cbl {
    int cb_avail;
    int cb_wait;

    struct cb *start;
    struct cb *front, *rear;
};

// Receive Frame Descriptor
struct rfd {
    volatile uint16_t rfd_status;
    uint16_t rfd_control;
    uint32_t rfd_link;

    uint32_t rfd_reserved;
    uint16_t rfd_actual_count;
    uint16_t rfd_size;

    char rfd_data[RFD_MAXSIZE];


    struct rfd *prev, *next;
    physaddr_t phy_addr;   
};


// Receive Frame Area
struct rfa {
    int rfd_avail;
    int rfd_wait;

    struct rfd *start;
    struct rfd *front, *rear;
};

// Network Interface Card
struct nic {
    uint32_t io_base;
    uint32_t io_size;

    struct cbl cbl;
    struct rfa rfa;
};

/* pci_attach_vendor matches the vendor ID and device ID of a PCI device */
int e100_attach(pci_pdata_t pd);
int e100_transmit(const char *data, uint16_t len);
int e100_receive(char *data);

#endif /* NET_E100_H */
