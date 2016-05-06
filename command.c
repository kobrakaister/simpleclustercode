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

struct node_struct nodes[100];
static int nnodes=0;

int send_command(int sockfd,char *command,char *dir_name,int cpus)
{

    char sdbuf[LENGTH]; // Receiver buffer

	bzero(sdbuf, LENGTH);

	sprintf(sdbuf,"gpvdmcommand\n#exe_name\n%s\n#dir_name\n%s\n#cpus\n%d\n#end",command,dir_name,cpus);

    if(send_all(sockfd, sdbuf, LENGTH) < 0)
    {
		printf("%s\n", strerror(errno));
	    return -1;
    }

return 0;
}

int cmp_node_runjob(int sock,char *revbuf)
{
	char exe_name[200];
	char dir_name[200];
	char buf[LENGTH];
	int cpus=0;
	if (cmpstr_min(revbuf,"gpvdmcommand")==0)
	{

		struct inp_file decode;
		printf("revbuf='%s' %d\n",revbuf,cmpstr_min(revbuf,"gpvdmcommand"));
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		inp_search_string(&decode,exe_name,"#exe_name");
		inp_search_string(&decode,dir_name,"#dir_name");
		inp_search_int(&decode,&cpus,"#cpus");
		printf("I will run %s\n",exe_name);
		if (fork()==0)
		{

			char sim_dir[200];
			join_path(2,sim_dir,calpath_get_store_path(), dir_name);
			printf("change dir to %s\n",sim_dir);
			chdir(sim_dir);

			char full_exe_path[400];
			char lib_path[400];
			char command[400];
			join_path(3,full_exe_path,calpath_get_store_path(), "src",exe_name);
			join_path(2,lib_path,calpath_get_store_path(), "src");
			sprintf(command,"export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:%s;%s",lib_path,full_exe_path);
			printf("full command =%s\n",command);
			system(command);
			
			bzero(buf, LENGTH);


			//send_dir(sock, sim_dir, 0, sim_dir, dir_name);

			printf("FINISHED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %s\n",dir_name);

			sprintf(buf,"gpvdmsimfinished\n#dir_name\n%s\n#cpus\n%d\n#ip\n%s\n#end",dir_name,cpus,get_my_ip());

			if(send_all(sock, buf, LENGTH) < 0)
			{
				printf("%s\n", strerror(errno));
				return -1;
			}
			 _exit(EXIT_SUCCESS);
		}
	}

return -1;
}

int cmp_head_exe(int sock,char *revbuf)
{
	char command[200];
	char dir_name[200];
	char buf[LENGTH];
	int cpus=0;
	if (cmpstr_min(revbuf,"gpvdmheadex")==0)
	{
		struct inp_file decode;
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		inp_search_string(&decode,dir_name,"#dir");
		inp_search_string(&decode,command,"#command");

		printf("I will run %s\n",command);
		if (fork()==0)
		{

			char sim_dir[200];
			join_path(2,sim_dir,calpath_get_store_path(), dir_name);
			printf("change dir to %s\n",sim_dir);
			chdir(sim_dir);

			system(command);

			 _exit(EXIT_SUCCESS);
		}
	}

return -1;
}

