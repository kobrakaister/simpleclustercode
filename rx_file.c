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
#include <sys/stat.h>
static int count=0;
int cmp_rxfile(int sock_han,char *revbuf)
{
char save_name[200];
struct inp_file decode;
char dir_name[400];
char target[400];
char full_path[400]; //full path
int save_size;
int stat_value;
	if (cmpstr_min(revbuf,"gpvdmfile")==0)
	{
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		//printf("%s\n",revbuf);
		inp_search_string(&decode,save_name,"#file_name");
		inp_search_string(&decode,target,"#target");
		inp_search_int(&decode,&save_size,"#file_size");
		inp_search_int(&decode,&stat_value,"#stat");
		//printf("'%s' '%s'\n",save_name,target);

		cal_abs_path_from_target(full_path,target,save_name);
		
		count++;

		get_dir_name_from_path(dir_name, full_path);
		printf("file: %s %s\n",dir_name, full_path);

		file_rx_and_save(full_path,sock_han,save_size);

		if (stat_value!=0)
		{
			chmod(full_path,stat_value);
		}
		return 0;
		
	}

return -1;
}

int recv_all(int sock,char *buf, int buf_len)
{
char *ptr=buf;
int rx_len=0;
int to_get=buf_len;
int ret=0;

while(rx_len<buf_len)
{
	ret = recv(sock, ptr, to_get, MSG_WAITALL);
	if (ret<=0)
	{
		return ret;
	}

	ptr+=ret;
	to_get-=ret;
	rx_len+=ret;
}
return rx_len;
}

int file_rx_and_save(char *file_name,int sock_han,int size)
{
	int buf_len=((size/LENGTH)+1)*LENGTH;
	char *buf=malloc(buf_len*sizeof(char));
	mkdirs(file_name);
    //char revbuf[LENGTH];
	bzero(buf, LENGTH);
	int f_block_sz=0;
	int write_block_size=0;
	int written=0;
	int write_sz=0;

	FILE *fp = fopen(file_name, "w");
	if(fp == NULL)
	{
		printf(" %s\n", strerror(errno));
		return (0);
	}

	if (size!=0)
	{
		f_block_sz = recv_all(sock_han, buf, buf_len);

		decrypt(buf,buf_len);

		if(f_block_sz != buf_len)
		{
			printf("Not got all the data %s %d %d\n",file_name,f_block_sz, buf_len);
			return -1;
		}
		write_sz = fwrite(buf, sizeof(char), size, fp);
	}


	if(write_sz != size)
	{
		printf("Not wirtten all the data.\n");
		return -1;
	}

	fclose(fp);

	free(buf);
return 0;
}
