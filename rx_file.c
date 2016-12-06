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
#include <sys/stat.h>

#include "tx_packet.h"

static int count=0;
int cmp_rxfile(int sock,struct tx_struct *data)
{
int ret=0;
struct inp_file decode;
char dir_name[400];
char full_path[400]; //full path

	if (cmpstr_min(data->id,"gpvdmfile")==0)
	{
		char *buf=NULL;

		cal_abs_path_from_target(full_path,data->target,data->file_name);
		get_dir_name_from_path(dir_name, full_path);

		mkdirs(full_path);

		printf("file: %s %s\n",dir_name, full_path);


		FILE *fp = fopen(full_path, "w");

		if(fp == NULL)
		{
			printf("the error is %s\n", strerror(errno));
			return (0);
		}

		if (data->size>0)
		{
			fwrite(data->data, sizeof(char), data->size, fp);
		}

		fclose(fp);


		if (data->stat!=0)
		{
			chmod(full_path,data->stat);
		}

		if (data->size>0)
		{
			free(data->data);
			data->data=NULL;
		}

		count++;
		return 0;
		
		
	}

return -1;
}


