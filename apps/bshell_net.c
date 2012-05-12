#include "bshell.h"
#include "stdio.h"
#include "net.h"

void command_net(struct session *s) {
	int r = 0;
	r = show_nic();
}


void command_net_tx(struct session *s) {
	int r = 0;
	r = net_transmit("Hello", 5);
	printf("tx: 1 with net_transmit\n",r);
	s->cons->y++;
}

