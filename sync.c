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

#include <openssl/md5.h>
#include <openssl/aes.h>
#include <openssl/sha.h>

static int count=0;


void get_md5sum(char *ret,char *file_name)
{

FILE *file;
char *buf=NULL;
unsigned long len;
long l;
char chkfile[100];
unsigned char md5[16];


FILE *f = fopen(file_name, "rb");
if (f==NULL)
{
	printf("%s not found\n",file_name);
	strcpy(ret,"");
	return;
}

fseek(f, 0, SEEK_END);
len = ftell(f);
fseek(f, 0, SEEK_SET);

buf = malloc(len*sizeof(char));
fread(buf, len, 1, f);
fclose(f);


MD5((const unsigned char*)buf, len,md5);

sprintf(ret,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",md5[0],md5[1],md5[2],md5[3],md5[4],md5[5],md5[6],md5[7],md5[8],md5[9],md5[10],md5[11],md5[12],md5[13],md5[14],md5[15]);

free(buf);

return;
}

int cmp_sync_packet_one(int sock,char *revbuf)
{
struct inp_file decode;

int save_size;
int f_block_sz;
char target[200];
char src[200];
char full_path[400];

	if (cmpstr_min(revbuf,"gpvdm_sync_packet_one")==0)
	{
		printf("rx sync packet\n");
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);

		inp_search_int(&decode,&save_size,"#size");
		inp_search_string(&decode,target,"#target");
		inp_search_string(&decode,src,"#src");

		int buf_len=((save_size/LENGTH)+1)*LENGTH;
		char *buf=malloc(buf_len*sizeof(char));
		bzero(buf, buf_len);

		f_block_sz = recv_all(sock, buf, buf_len);

		decrypt(buf,buf_len);

		if(f_block_sz != buf_len)
		{
			printf("Not got all the data %s %d %d\n",f_block_sz, buf_len);
			return -1;
		}

		inp_init(&decode);
		decode.data=buf;
		decode.fsize=strlen(buf);

		char *fname=(char *)-1;
		char fname_buf[400];

		char *md5=(char *)-1;
		char md5_buf[400];
		char *build=NULL;
		int len=LENGTH*2;
		build=malloc(sizeof(char)*len);
		bzero(build, len);

		int pos=0;
		char seg[1000];
		char file_sum[64];
		int sent=0;
		int items=0;

		while(1)
		{
			fname  = inp_get_string(&decode);

			if (fname==NULL)
			{
				break;
			}
			printf("a=%s\n",fname);
			strcpy(fname_buf,fname);

			md5  = inp_get_string(&decode);		
			if (md5==NULL)
			{
				break;
			}
			strcpy(md5_buf,md5);

			cal_abs_path_from_target(full_path,target,fname_buf);

			get_md5sum(file_sum,full_path);

			int needed=TRUE;
			printf("a=%s\n",file_sum);
			printf("b=%s\n",md5_buf);

			if (strcmp(file_sum,md5_buf)==0)
			{
				needed=FALSE;
			}

			if (needed==TRUE)
			{
				sprintf(seg,"%s\n",fname_buf);
				int seg_len=strlen(seg);

				if (seg_len+pos>(len-LENGTH))
				{
					len+=LENGTH;
					build=realloc(build,sizeof(char)*(len));
					bzero((build+len-LENGTH), LENGTH);

				}

				strcat((build+LENGTH),seg);
				pos+=seg_len;
				items++;
			}

		}
		if (items!=0)
		{
			pos=pos-1;
		}

		sprintf(build,"gpvdm_sync_packet_two\n#size\n%d\n#target\n%s\n#src\n%s\n#end",pos,target,src);

		printf("b=%s\n",(build+LENGTH));

		sent=send_all(sock, build, len,TRUE);
		if(sent < 0)
		{
			printf("%s\n", strerror(errno));
			return -1;
		}

		free(build);
		free(buf);		

		return 0;
		
	}

return -1;
}

