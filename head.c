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
#include<pthread.h>

static int global_sock;

void head_ctrl_c(int s)
{
	close_all_open();
	printf("Caught signal %d\n",s);
	exit(1); 
}

int head ()
{

int port=0;
char file_store_path[200];
char ip[20];
char interface[400]; //full path
struct inp_file inp;
inp_init(&inp);
if (inp_load(&inp,"head.inp")!=0)
{
	printf("can't find file config.inp");
	exit(0);
}
inp_check(&inp,1.0);
inp_search_int(&inp,&port,"#port");
inp_search_string(&inp,ip,"#ip");
inp_search_string(&inp,file_store_path,"#file_store");
inp_search_string(&inp,interface,"#interface");
inp_free(&inp);

calpath_set_store_path(file_store_path);
    int sockfd; // Socket file descriptor
    int nsockfd; // New Socket file descriptor
    int num;
    int sin_size; // to store struct size
    struct sockaddr_in addr_local;
    struct sockaddr_in addr_remote;
    /* Get the Socket file descriptor */
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        printf ("ERROR: Failed to obtain Socket Descriptor.\n");
        return (0);
    }
    else
	{
		printf ("server: obtain socket descriptor successfully.\n");
	}
    /* Fill the local socket address struct */

    addr_local.sin_family = AF_INET; // Protocol Family
    addr_local.sin_port = htons(port); // Port number
    addr_local.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
    bzero(&(addr_local.sin_zero), 8); // Flush the rest of struct
    /* Bind a special Port */

    if( bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1 )
    {
        printf ("ERROR: Failed to bind Port %d.\n",port);
        return (0);
    }
    else
	{
		printf("server: bind tcp port %d in addr 0.0.0.0 sucessfully.\n",port);
	}


    /* Listen remote connect/calling */
    if(listen(sockfd,BACKLOG) == -1)
    {
        printf ("ERROR: Failed to listen Port %d.\n", port);
        return (0);
    }
    else
	{
		printf ("server: listening the port %d sucessfully.\n", port);
	}

    int success=0;



	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = head_ctrl_c;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

    while(success == 0)
    {
        sin_size = sizeof(struct sockaddr_in);
        /* Wait a connection, and obtain a new socket file despriptor for single connection */
        if ((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, &sin_size)) == -1)
		{
            printf ("ERROR: Obtain new Socket Despcritor error.\n");
		}
        else
		{
			printf ("[server] server has got connect from %s.\n", inet_ntoa(addr_remote.sin_addr));
		}

		pthread_t thread1;
		int iret1;

		iret1 = pthread_create( &thread1, NULL,rx_loop,(void*)&nsockfd);

     
    }

}
