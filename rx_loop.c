//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	https://www.gpvdm.com
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
int ret=0;
int processed=FALSE;
struct tx_struct data;
int sock=*((int*)s);


	while(1)
	{
		processed=FALSE;
		ret=rx_packet(sock,&data);

		if (ret==-1)
		{
			printf("Breaking due to -1\n");
			break;
		}
		
		if (cmp_rxfile(sock,&data)==0)
		{
			printf("a\n");
			processed=TRUE;
		}else
		if (cmp_addjob(sock,&data)==0)
		{
			printf("b\n");
			processed=TRUE;
		}else
		if (cmp_addnode(sock,&data)==0)
		{
			printf("c\n");
			processed=TRUE;
		}else
		if (cmp_deletenode(sock,&data)==0)
		{
			printf("d\n");
			processed=TRUE;
		}else
		if (cmp_runjobs(sock,&data)==0)
		{
			printf("e\n");
			processed=TRUE;
		}else
		if (cmp_simfinished(sock,&data)==0)
		{
			printf("f\n");
			processed=TRUE;
		}else
		if (cmp_register_master(sock,&data)==0)
		{
			printf("g\n");
			processed=TRUE;
		}else
		if (cmp_head_killall(sock,&data)==0)
		{
			printf("h\n");
			processed=TRUE;
		}else
		if (cmp_head_sleep(sock,&data)==0)
		{
			printf("i\n");
			processed=TRUE;
		}else
		if (cmp_head_poweroff(sock,&data)==0)
		{
			printf("j\n");
			processed=TRUE;
		}else
		if (cmp_sendnodelist(sock,&data)==0)
		{
			printf("k\n");
			processed=TRUE;
		}else
		if (cmp_get_data(sock,&data)==0)
		{
			printf("l\n");
			processed=TRUE;
		}else
		if (cmp_head_exe(sock,&data)==0)
		{
			printf("m\n");
			processed=TRUE;
		}else
		if (cmp_master_clean(sock,&data)==0)
		{
			printf("n\n");
			processed=TRUE;
		}else
		if (cmp_rxloadstats(sock,&data)==0)
		{
			printf("o\n");
			processed=TRUE;
		}else
		if (cmp_head_quit(sock,&data)==0)
		{
			printf("p\n");
			processed=TRUE;
		}else
		if (cmp_rxsetmaxloads(sock,&data)==0)
		{
			printf("1\n");
			processed=TRUE;
		}else
		if (cmp_sync_packet_one(sock,&data)==0)
		{
			printf("2\n");
			processed=TRUE;
		}else
		if (cmp_sync_packet_two(sock,&data)==0)
		{
			printf("3\n");
			processed=TRUE;
		}else
		if (cmp_send_job_list(sock,&data)==0)
		{
			printf("4\n");
			processed=TRUE;
		}else
		if (cmp_head_stop_all_jobs(sock,&data)==0)
		{
			printf("5\n");
			processed=TRUE;
		}else
		if (cmp_delete_all_jobs(sock,&data)==0)
		{
			printf("6\n");
			processed=TRUE;
		}
		
		if (processed==FALSE)
		{
			printf("Command not understood!!!!!!!!!!!!!!!!! %s\n",data.id);
			getchar();
		}
		jobs_save();

	}

	

close(sock);
printf("leavng main rx loop\n");
return NULL;
}
