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
#include "tx_packet.h"

struct node_struct nodes[100];
static int nnodes=0;

int send_command(int sock,char *command,char *dir_name,int cpus)
{
	int ret=0;
	struct tx_struct packet;
	tx_struct_init(&packet);
	tx_set_id(&packet,"gpvdmcommand");
	strcpy(packet.exe_name,command);
	strcpy(packet.dir_name,dir_name);
	packet.cpus=cpus;

	ret=tx_packet(sock,&packet,NULL);


return ret;
}

int cmp_node_runjob(int sock,struct tx_struct *data)
{
	int ret=0;
	if (cmpstr_min(data->id,"gpvdmcommand")==0)
	{

		printf("I will run %s in a new process\n",data->exe_name);
		if (fork()==0)
		{

			char sim_dir[200];
			join_path(2,sim_dir,calpath_get_store_path(), data->dir_name);
			printf("change dir to %s\n",sim_dir);
			chdir(sim_dir);

			char full_exe_path[400];
			char lib_path[400];
			char command[400];
			join_path(3,full_exe_path,calpath_get_store_path(), "src",data->exe_name);
			join_path(2,lib_path,calpath_get_store_path(), "src");
			sprintf(command,"export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:%s;stdbuf -i0 -o0 -e0 %s",lib_path,full_exe_path);
			printf("full command =%s\n",command);
			system(command);
			


			//send_dir(sock, sim_dir, 0, sim_dir, data->dir_name);

			printf("FINISHED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %s\n",data->dir_name);

			struct tx_struct packet;
			tx_struct_init(&packet);
			tx_set_id(&packet,"gpvdmsimfinished");
			strcpy(packet.dir_name,data->dir_name);
			strcpy(packet.ip,get_my_ip());
			packet.cpus=data->cpus;

			tx_packet(sock,&packet,NULL);

			 _exit(EXIT_SUCCESS);
		}
		
		return 0;
	}

return -1;
}

int cmp_head_exe(int sock,struct tx_struct *data)
{
	int ret=0;
	if (cmpstr_min(data->id,"gpvdmheadex")==0)
	{


		printf("I will run %s\n",data->command);
		if (fork()==0)
		{

			char sim_dir[200];
			join_path(2,sim_dir,calpath_get_store_path(), data->dir_name);
			printf("change dir to %s\n",sim_dir);
			chdir(sim_dir);

			system(data->command);

			send_message("head node says done.");

			 _exit(EXIT_SUCCESS);
		}
	}

return -1;
}

