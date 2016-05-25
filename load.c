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
#include <time.h>
#include "tx_packet.h"

struct node_struct nodes[100];
static int nnodes=0;

int send_node_load(int sock)
{
	double loadavg[3];
	getloadavg(loadavg, 3);

	struct tx_struct packet;
	tx_struct_init(&packet);
	tx_set_id(&packet,"gpvdmload");
	tx_set_size(&packet,0);
	packet.load0=loadavg[0];
	packet.load1=loadavg[1];
	packet.load2=loadavg[2];
	strcpy(packet.ip,get_my_ip());

	tx_packet(sock,&packet,NULL);

return 0;
}

int cmp_nodeload(int sock,char *revbuf)
{
	char exe_name[200];
	char dir_name[200];
	char buf[LENGTH];
	int cpus=0;

	int ret=0;
	if (cmpstr_min(revbuf,"gpvdmnodegetload")==0)
	{
		ret=send_node_load(sock);
		return ret;
	}
return -1;
}

int cmp_rxloadstats(int sock,char *revbuf)
{
	int ret;
	struct tx_struct data;
	struct node_struct* node;
	if (cmpstr_min(revbuf,"gpvdmload")==0)
	{
		ret=rx_packet(sock,&data,revbuf);

		node=node_find(data.ip);
		if (node!=NULL)
		{
			node->load0=data.load0;
			node->alive=time(NULL);
		}
	}

return -1;
}

int cmp_rxsetmaxloads(int sock,char *revbuf)
{
//printf("test send list %s\n",revbuf);
char buf[LENGTH];
int i;
char *node_ip=NULL;
char node_buf[100];
char *max_load=NULL;
char max_buf[100];
int cpus=0.0;
int ret=0;

	if (cmpstr_min(revbuf,"gpvdm_set_max_loads")==0)
	{
		struct tx_struct data;
		char *buf=NULL;
		ret=rx_packet(sock,&data,revbuf);

		printf("rod %s %s\n",revbuf,data.data);
		struct inp_file in;
		inp_init(&in);
		in.data=data.data;
		in.fsize=data.size;
		inp_reset_read(&in);

		max_load  = (char*)-1;

		while(max_load!=NULL)
		{
			node_ip  = inp_get_string(&in);
			if (node_ip==NULL)
			{
				break;
			}
			strcpy(node_buf,node_ip);

			max_load  = inp_get_string(&in);		
			if (max_load==NULL)
			{
				break;
			}
			strcpy(max_buf,max_load);

			printf("node ip %s\n",node_buf);
			printf("max_load %s\n",max_buf);
			struct node_struct* node=NULL;
			node=node_find(node_buf);

			if (node==NULL)
			{
				printf("I can't find IP %s\n",node_buf);
			}

			sscanf(max_buf,"%d",&cpus);
			printf("max found as%d %s\n",cpus,node->host_name);
			node->max_cpus=cpus;

			nodes_print();
		}

		nodes_print();

		return 0;
	}

return -1;
}


