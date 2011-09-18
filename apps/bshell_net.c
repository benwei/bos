#include "bshell.h"
#include "stdio.h"
#include "net.h"

void command_net(struct session *s) {
	int r = 0;
	new_line(s);
	r = show_nic();
	s->cons->y+=r;
}


void command_net_tx(struct session *s) {
	int r = 0;
	new_line(s);
	r = net_transmit("Hello", 5);
	printf("tx: 1 with net_transmit\n",r);
	s->cons->y++;
}

