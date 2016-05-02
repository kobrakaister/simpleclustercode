#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "inp.h"
#include "util.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include "inp.h"

void *rx_loop(void *s)
{
int sock=*((int*)s);
	int f_block_sz = 0;
    char revbuf[LENGTH];

	while(f_block_sz = recv(sock, revbuf, LENGTH, 0))
	{


		if(f_block_sz < 0)
		{
			printf("here %s\n", strerror(errno));
			return NULL;
		}

		cmp_rxfile(sock,revbuf);

		cmp_addjob(sock,revbuf);

		cmp_addnode(sock,revbuf);

		cmp_deletenode(sock,revbuf);

		cmp_command(sock,revbuf);

		cmp_runjobs(sock,revbuf);

		cmp_simfinished(sock,revbuf);

		cmp_register_master(sock,revbuf);

		cmp_head_killall(sock,revbuf);

		cmp_head_sleep(sock,revbuf);

		cmp_head_poweroff(sock,revbuf);

		cmp_sendnodelist(sock,revbuf);

		cmp_get_data(sock,revbuf);

	}

	bzero(revbuf, LENGTH);

close(sock);
return NULL;
}
