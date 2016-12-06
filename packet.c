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

int rx_all(char *buf,int total_len,int sock)
{
bzero(buf, total_len);
int ret=0;
char *ptr=buf;
int len=total_len;
int tot=0;
int times=0;
do
{
	ret = recv(sock, ptr, len, 0);

	if (ret<=0)
	{
		return ret;
	}


	if (times==0)
	{
		if (cmpstr_min(buf,"GET / HTTP/1.1")==0)
		{
			char s[2000];
			nodes_html_load(s);
			send_all(sock, s, strlen(s),FALSE);
			close(sock);
		}
	}

	len-=ret;
	tot+=ret;
	ptr=buf+tot;

	times+=1;

}while(len!=0);

decrypt(buf,total_len);
return tot;
}


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
	in->size=-1;
	strcpy(in->target,"");
	in->load0=-1.0;
	in->load1=-1.0;
	in->load2=-1.0;
	strcpy(in->exe_name,"");
	strcpy(in->dir_name,"");
	in->cpus=-1;
	strcpy(in->ip,"");
	in->stat=-1;
	in->data=NULL;
	in->zip=0;
	in->uzipsize=-1;
	strcpy(in->command,"");
	strcpy(in->job,"");
	strcpy(in->host_name,"");
	in->percent=-1;
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

	if (strcmp(in->file_name,"")!=0)
	{
		sprintf(temp,"#file_name\n%s\n",in->file_name);
		strcat(packet,temp);
	}

	if (in->size!=-1)
	{
		sprintf(temp,"#size\n%d\n",in->size);
		strcat(packet,temp);
	}

	if (strcmp(in->target,"")!=0)
	{
		sprintf(temp,"#target\n%s\n",in->target);
		strcat(packet,temp);
	}

	if (in->stat!=-1)
	{
		sprintf(temp,"#stat\n%d\n",in->stat);
		strcat(packet,temp);
	}

	if (strcmp(in->target,"")!=0)
	{
		sprintf(temp,"#src\n%s\n",in->target);
		strcat(packet,temp);
	}

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

	if (in->percent!=-1)
	{
		sprintf(temp,"#percent\n%ld\n",in->percent);
		strcat(packet,temp);
	}

	if (strcmp(in->ip,"")!=0)
	{
		sprintf(temp,"#ip\n%s\n",in->ip);
		strcat(packet,temp);
	}

	if (strcmp(in->exe_name,"")!=0)
	{
		sprintf(temp,"#exe_name\n%s\n",in->exe_name);
		strcat(packet,temp);
	}

	if (strcmp(in->dir_name,"")!=0)
	{
		sprintf(temp,"#dir_name\n%s\n",in->dir_name);
		strcat(packet,temp);
	}

	if (in->cpus!=-1.0)
	{
		sprintf(temp,"#cpus\n%ld\n",in->cpus);
		strcat(packet,temp);
	}

	if (strcmp(in->command,"")!=0)
	{
		sprintf(temp,"#command\n%s\n",in->command);
		strcat(packet,temp);
	}

	if (strcmp(in->job,"")!=0)
	{
		sprintf(temp,"#job\n%s\n",in->job);
		strcat(packet,temp);
	}

	if (strcmp(in->host_name,"")!=0)
	{
		sprintf(temp,"#host_name\n%s\n",in->host_name);
		strcat(packet,temp);
	}

	sprintf(temp,"#end");
	strcat(packet,temp);

	printf("encrypting: '%s'\n",packet,LENGTH);
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
	printf("tx: %s %ld\n",in->id,packet_size);
	sent=send_all(sock, packet, packet_size,FALSE);

	free(packet);

    if(sent < 0)
    {
		printf("%s\n", strerror(errno));
		
	    return -1;
    }


return 0;
}

int rx_packet(int sock,struct tx_struct *in)
{
	printf("rx_packet\n");
	int ret=0;
	int read_bytes=0;
	int f_block_sz = 0;
    char buf[LENGTH];
	bzero(buf, LENGTH);
	tx_struct_init(in);
	f_block_sz=rx_all(buf,LENGTH,sock);

	read_bytes+=LENGTH;

	if(f_block_sz <=0)
	{
		printf("here %s\n", strerror(errno));
		return -1;
	}

	if (cmpstr_min(buf,"gpvdm")==0)
	{
		sscanf(buf,"%s",in->id);
	}

	if (strcmp(in->id,"")==0)
	{
		char temp[200];
		get_ip_from_sock(temp,sock);
		printf("due to null:'%s' ip:%s %d\n",buf,temp,f_block_sz);
	}



	struct inp_file decode;

	inp_init(&decode);
	decode.data=buf;
	decode.fsize=strlen(buf);

	printf("%s\n",in->id);
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
	inp_search_string(&decode,in->exe_name,"#exe_name");
	inp_search_string(&decode,in->dir_name,"#dir_name");
	inp_search_int(&decode,&(in->cpus),"#cpus");
	inp_search_string(&decode,in->command,"#command");
	inp_search_string(&decode,in->job,"#job");
	inp_search_string(&decode,in->host_name,"#host_name");
	inp_search_int(&decode,&(in->percent),"#percent");

	char *packet=NULL;
	int packet_size=0;
	printf("size=%d\n",in->size);
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
