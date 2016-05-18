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
#include "util.h"
#include "tx_packet.h"


void tx_struct_init(struct tx_struct *in)
{
	strcpy(in->id,"");
	strcpy(in->src,"");
	strcpy(in->file_name,"");
	in->size=0;
	strcpy(in->target,"");
	in->stat=0;
}

void tx_set_id(struct tx_struct *in,char *id)
{
	strcpy(in->id,id);
}

void tx_set_size(struct tx_struct *in,int size)
{
	in->size=size;
}

void tx_set_stat(struct tx_struct *in,int stat_in)
{
	in->stat=stat_in;
}

void tx_set_file_name(struct tx_struct *in,char *file_name)
{
	strcpy(in->file_name,file_name);
}

void tx_set_target(struct tx_struct *in,char *target)
{
	strcpy(in->target,target);
}

int tx_packet(int sock,struct tx_struct *in,char *buf)
{
	char *packet=NULL;
	int packet_size=0;

	if (in->size>0)
	{
		packet_size=((((int)in->size)/((int)LENGTH))+2)*LENGTH;
	}else
	{
		packet_size=((((int)in->size)/((int)LENGTH))+1)*LENGTH;
	}

	packet=(char*)malloc(sizeof(char)*packet_size);
	bzero(packet, packet_size);

	sprintf(packet,"%s\n#file_name\n%s\n#file_size\n%d\n#target\n%s\n#stat\n%d\n#end",in->id,in->file_name,in->size,in->target,in->stat);

	//printf("sending %s\n",packet);
	encrypt(packet,LENGTH);
	

	if (in->size>0)
	{
		memcpy((packet+LENGTH), buf, in->size);
		encrypt((packet+LENGTH),(packet_size-LENGTH));
	}

	int sent=0;
	sent=send_all(sock, packet, packet_size,FALSE);

	free(packet);

    if(sent < 0)
    {
		printf("%s\n", strerror(errno));
		
	    return -1;
    }


return 0;
}
