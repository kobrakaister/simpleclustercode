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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>

#include "util.h"
#include "inp.h"

static int global_sock=0;


void my_handler(int s)
{
	send_delete_node(global_sock);
	printf("Caught signal %d\n",s);
	exit(1); 
}

int node()
{

char node_name[100];
char store_path[200];
char temp_path[200];
char interface[200];
char master_ip[200];
int port=0;
struct inp_file inp;
inp_init(&inp);
if (inp_load(&inp,"node.inp")!=0)
{
	printf("can't find file config.inp");
	exit(0);
}
inp_check(&inp,1.0);
inp_search_string(&inp,node_name,"#node_name");
inp_search_string(&inp,temp_path,"#store_path");
inp_search_string(&inp,interface,"#interface");
inp_search_string(&inp,master_ip,"#master_ip");
inp_search_int(&inp,&port,"#port");
inp_free(&inp);
realpath(temp_path, store_path);
calpath_set_store_path(store_path);


    int sockfd; // Socket file descriptor
    int sin_size; // to store struct size
    int nsockfd; // New Socket file descriptor
    struct sockaddr_in addr_remote;
    struct sockaddr_in remote_addr; 
    /* Get the Socket file descriptor */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
		printf(" %s\n", strerror(errno));
        return (0);
    }
    /* Fill the socket address struct */
    remote_addr.sin_family = AF_INET; 
    remote_addr.sin_port = htons(port); 
    inet_pton(AF_INET, master_ip, &remote_addr.sin_addr); 
    bzero(&(remote_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
    {
		printf(" %s\n", strerror(errno));
        return (0);
    }
    else
	{
		printf("client: Connected to server at port %d...ok!\n", port);
	}


	char revbuf[LENGTH];
	int f_block_sz;

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	global_sock=sockfd;

	cal_my_ip(sockfd);

	register_node(sockfd,node_name);


	while(f_block_sz = recv(sockfd, revbuf, LENGTH, MSG_WAITALL))
	{

		//printf("%s\n",revbuf);

		cmp_node_runjob(sockfd,revbuf);

		cmp_rxfile(sockfd,revbuf);

		cmp_node_killall(sockfd,revbuf);

		cmp_node_sleep(sockfd,revbuf);

		cmp_node_poweroff(sockfd,revbuf);

		cmp_node_send_data(sockfd,revbuf);

		if(f_block_sz < 0)
		{
			printf(" %s\n", strerror(errno));
			return (0);
		}

		
	}
/////////////////////////////////////////


    close (sockfd);
    printf("client: connection lost.\n");
    return (0);
}
