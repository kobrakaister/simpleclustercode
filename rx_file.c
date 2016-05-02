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

int cmp_rxfile(int sock_han,char *revbuf)
{
char save_name[200];
struct inp_file decode;
char dir_name[400];
char target[400];
char full_path[400]; //full path
int save_size;

	if (cmpstr_min(revbuf,"gpvdmfile")==0)
	{
		//printf("%s\n",revbuf);
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		inp_search_string(&decode,save_name,"#file_name");
		inp_search_string(&decode,target,"#target");
		inp_search_int(&decode,&save_size,"#file_size");
		//printf("'%s' '%s'\n",save_name,target);
		struct job* job=NULL;
		job=jobs_find_job(target);
		if (job!=NULL)
		{
			join_path(3,full_path,calpath_get_store_path(),job->name,save_name);
			printf("full path=%s %s\n",full_path,target);
			//getchar();
			get_dir_name_from_path(dir_name, full_path);

			file_rx_and_save(full_path,sock_han,save_size);
		return 0;
		}else
		{
			printf("job for target '%s' not found\n",target);
		}
	}

return -1;
}

int file_rx_and_save(char *file_name,int sock_han,int size)
{
	mkdirs(file_name);
    char revbuf[LENGTH];
	bzero(revbuf, LENGTH);
	int f_block_sz=0;
	int write_block_size=0;
	int written=0;

	FILE *fp = fopen(file_name, "w");
	if(fp == NULL)
	{
		printf(" %s\n", strerror(errno));
		return (0);
	}

	if (size==0)
	{
		printf("File size 0 of %s\n",file_name);
		fclose(fp);
		return 0;
	}

	written=0;
	while(f_block_sz = recv(sock_han, revbuf, LENGTH, 0))
	{

		int left=size-written;
		if (left<LENGTH)
		{
			write_block_size=left;
		}else
		{
			write_block_size=LENGTH;
		}

		if(f_block_sz < 0)
		{
			printf(" %s\n", strerror(errno));
			return -1;
		}

		int write_sz = fwrite(revbuf, sizeof(char), write_block_size, fp);

		if(write_sz != write_block_size)
		{
			printf("File write failed.\n");
			break;
		}

		written+=write_sz;
		if (written==size)
		{
			break;
		}
		bzero(revbuf, LENGTH);
	}

	fclose(fp);

return 0;
}
