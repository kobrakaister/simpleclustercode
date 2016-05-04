//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.roderickmackenzie.eu
//	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.

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

	while(f_block_sz = recv(sock, revbuf, LENGTH, MSG_WAITALL))
	{

		printf("block_size=%s\n",revbuf);
		if(f_block_sz < 0)
		{
			printf("here %s\n", strerror(errno));
			return NULL;
		}

		cmp_rxfile(sock,revbuf);

		cmp_addjob(sock,revbuf);

		cmp_addnode(sock,revbuf);

		cmp_deletenode(sock,revbuf);

		cmp_runjobs(sock,revbuf);

		cmp_simfinished(sock,revbuf);

		cmp_register_master(sock,revbuf);

		cmp_head_killall(sock,revbuf);

		cmp_head_sleep(sock,revbuf);

		cmp_head_poweroff(sock,revbuf);

		cmp_sendnodelist(sock,revbuf);

		cmp_get_data(sock,revbuf);

		cmp_head_exe(sock,revbuf);

		cmp_clean(sock,revbuf);
	}

	bzero(revbuf, LENGTH);

close(sock);
printf("leavng main rx loop\n");
return NULL;
}
