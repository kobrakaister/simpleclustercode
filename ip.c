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
#include<pthread.h>

static char my_ip[20];

char* get_my_ip()
{
	return my_ip;
}

int cal_my_ip(char *interface)
{
   int fd;
    struct ifreq ifr;
     
    fd = socket(AF_INET, SOCK_DGRAM, 0);
 
    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
 
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , interface , IFNAMSIZ-1);
 
    ioctl(fd, SIOCGIFADDR, &ifr);
 
    close(fd);
 
    strcpy(my_ip,inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));
 
  return 0;
}

int get_ip_from_sock(char *out,int sock)
{
	int z;
	struct sockaddr_in adr_inet;
	int len_inet;

	len_inet = sizeof adr_inet;

	z = getpeername(sock, (struct sockaddr *)&adr_inet, &len_inet);
	if ( z == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	strcpy(out,inet_ntoa(adr_inet.sin_addr));
	return 0;
}
