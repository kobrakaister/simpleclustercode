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
#include "tx_packet.h"

int cmp_node_send_data(int sock,struct tx_struct *data)
{
	struct job* jobs=NULL;
	char full_path[200];
	int ret=0;
	struct tx_struct packet;

	if (cmpstr_min(data->id,"gpvdmnodesenddata")==0)
	{


		join_path(2,full_path,calpath_get_store_path(), data->job);

		printf("sending dir %s\n",full_path);
		send_dir(sock,full_path, 0,full_path,data->job);

		tx_struct_init(&packet);
		tx_set_id(&packet,"gpvdmgetdata");
		tx_packet(sock,&packet,NULL);

	}

return -1;
}


int cmp_get_data(int sock,struct tx_struct *data)
{
	int i=0;

	int njobs=0;
	struct job* jobs=NULL;
	char buf[512];
	char full_path[512];
	int state_changed=FALSE;
	if (cmpstr_min(data->id,"gpvdmgetdata")==0)
	{
		printf("check getdata gpvdmgetdata\n");

		jobs=get_jobs_array();
		njobs=get_njobs();
		
		for (i=0;i<njobs;i++)
		{
			if ((jobs[i].copy_state==0)&&(jobs[i].status>0))
			{

				struct node_struct* job_node=NULL;
				job_node=node_find(jobs[i].ip);

				struct tx_struct packet;
				tx_struct_init(&packet);
				tx_set_id(&packet,"gpvdmnodesenddata");
				sprintf(packet.job,"job%d",i);

				tx_packet(job_node->sock,&packet,NULL);

				state_changed=TRUE;
				jobs[i].copy_state=1;
				break;
			}
		}

		if (state_changed==FALSE)
		{
			for (i=0;i<njobs;i++)
			{
				jobs[i].copy_state=0;
			}
			jobs_print();
			printf("Got all data from nodes\n");


			for (i=0;i<njobs;i++)
			{
				join_path(2,full_path,calpath_get_store_path(), jobs[i].name);
				printf("tx to %s %s\n",full_path,jobs[i].target);
				struct node_struct* master=NULL;
				master=node_find_master();
				send_dir(master->sock,full_path, 0,full_path,jobs[i].target);

			}
		}		
	}

return -1;
}

