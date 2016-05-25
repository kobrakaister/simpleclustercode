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
#include "inp.h"
#include "tx_packet.h"

int send_all(int sock, void *buffer, int length, int encode)
{
//printf("sending to %d\n",sock);
	if (encode==TRUE)
	{
		encrypt(buffer,LENGTH);
	

		if (length>LENGTH)
		{
			encrypt((buffer+LENGTH),(length-LENGTH));
		}
	}
    int orig_length=length;
    char *ptr = (char*) buffer;
    while (length > 0)
    {
        int i = send(sock, ptr, length, 0);
        if (i < 1) return -1;
        ptr += i; 
        length -= i;
        if (length!=0)
        {
                printf("RESEND %d %d\n",length,orig_length);
        }
    }
//printf("end-sending to %d\n",sock);
    return 0;
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

void tx_struct_init(struct tx_struct *in)
{
	strcpy(in->id,"");
	strcpy(in->src,"");
	strcpy(in->file_name,"");
	strcpy(in->message,"");
	in->size=0;
	strcpy(in->target,"");
	in->load0=-1.0;
	in->load1=-1.0;
	in->load2=-1.0;
	strcpy(in->ip,"");
	in->stat=0;
	in->data=NULL;
	in->zip=0;
	in->uzipsize=0;
}

void tx_set_id(struct tx_struct *in,char *id)
{
	strcpy(in->id,id);
}

void tx_set_src(struct tx_struct *in,char *src)
{
	strcpy(in->src,src);
}


void tx_set_size(struct tx_struct *in,int size)
{
	in->size=size;
}

void tx_set_stat(struct tx_struct *in,int stat_in)
{
	in->stat=stat_in;
}

void tx_set_zip(struct tx_struct *in,int zip)
{
	in->zip=zip;
}

void tx_set_uzipsize(struct tx_struct *in,int uzipsize)
{
	in->uzipsize=uzipsize;
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
	char *zbuf=NULL;
	int packet_size=0;

	if (in->size>0)
	{
		if (in->zip==1)
		{
			long unsigned int comp_size=0;
			comp_size=(int)in->size+200;
			zbuf=malloc(sizeof(char)*comp_size);
			bzero(zbuf, comp_size);
			int ret=compress(zbuf, &comp_size, buf, in->size);
			in->uzipsize=in->size;
			in->size=(int)comp_size;
			printf("%d %d %d\n",comp_size,in->uzipsize,ret);
		}

		packet_size=((((int)in->size)/((int)LENGTH))+2)*LENGTH;
	}else
	{
		packet_size=((((int)in->size)/((int)LENGTH))+1)*LENGTH;
	}

	packet=(char*)malloc(sizeof(char)*packet_size);
	bzero(packet, packet_size);
	char temp[200];
	strcpy(packet,"");

	sprintf(temp,"%s\n",in->id);
	strcat(packet,temp);

	sprintf(temp,"#file_name\n%s\n",in->file_name);
	strcat(packet,temp);

	sprintf(temp,"#size\n%d\n",in->size);
	strcat(packet,temp);

	sprintf(temp,"#target\n%s\n",in->target);
	strcat(packet,temp);

	sprintf(temp,"#stat\n%d\n",in->stat);
	strcat(packet,temp);

	sprintf(temp,"#src\n%s\n",in->src);
	strcat(packet,temp);

	if (in->zip!=0)
	{
		sprintf(temp,"#zip\n%d\n",in->zip);
		strcat(packet,temp);

		sprintf(temp,"#uzipsize\n%d\n",in->uzipsize);
		strcat(packet,temp);
	}

	if (strcmp(in->message,"")!=0)
	{
		sprintf(temp,"#message\n%s\n",in->message);
		strcat(packet,temp);
	}

	if (in->load0!=-1.0)
	{
		sprintf(temp,"#load0\n%.3lf\n",in->load0);
		strcat(packet,temp);
	}

	if (in->load1!=-1.0)
	{
		sprintf(temp,"#load1\n%.3lf\n",in->load1);
		strcat(packet,temp);
	}

	if (in->load2!=-1.0)
	{
		sprintf(temp,"#load2\n%.3lf\n",in->load2);
		strcat(packet,temp);
	}

	if (strcmp(in->ip,"")!=0)
	{
		sprintf(temp,"#ip\n%s\n",in->ip);
		strcat(packet,temp);
	}

	sprintf(temp,"#end");
	strcat(packet,temp);

	//printf("sending %s\n",packet);
	encrypt(packet,LENGTH);
	

	if (in->size>0)
	{
		if (in->zip==1)
		{
			memcpy((packet+LENGTH), zbuf, in->size);
			free(zbuf);
		}else
		{
			memcpy((packet+LENGTH), buf, in->size);
		}
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

int rx_packet(int sock,struct tx_struct *in,char *buf)
{
	int ret=0;
	int read_bytes=0;

	struct inp_file decode;

	inp_init(&decode);
	decode.data=buf;
	decode.fsize=strlen(buf);

	inp_search_string(&decode,in->file_name,"#file_name");
	inp_search_string(&decode,in->target,"#target");
	inp_search_string(&decode,in->src,"#src");
	inp_search_int(&decode,&(in->size),"#size");
	inp_search_int(&decode,&(in->stat),"#stat");
	inp_search_int(&decode,&(in->zip),"#zip");
	inp_search_int(&decode,&(in->uzipsize),"#uzipsize");
	inp_search_double(&decode,&(in->load0),"#load0");
	inp_search_double(&decode,&(in->load1),"#load1");
	inp_search_double(&decode,&(in->load2),"#load2");
	inp_search_string(&decode,in->ip,"#ip");

	char *packet=NULL;
	int packet_size=0;

	if (in->size>0)
	{
		packet_size=((((int)in->size)/((int)LENGTH))+1)*LENGTH;
		in->data=(char*)malloc(sizeof(char)*packet_size);
		bzero(in->data, packet_size);

		read_bytes = recv_all(sock, in->data, packet_size);

		decrypt(in->data,packet_size);

		if(read_bytes != packet_size)
		{
			printf("Not got all the data: %d %d\n",read_bytes, packet_size);
			free(in->data);
			in->data=NULL;
			return -1;
		}

		if (in->zip==1)
		{
			char *uzip=(char*)malloc(sizeof(char)*in->uzipsize);
			long unsigned int s=0;
			s=in->uzipsize;
			uncompress(uzip, &(s), in->data, in->size);
			free(in->data);
			in->data=uzip;
			in->size=in->uzipsize;
		}
	}

return read_bytes;
}
