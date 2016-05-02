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

struct node_struct nodes[100];
static int nnodes=0;

int broadcast_to_nodes(int sock,char *command)
{
printf("broadcast\n");
int i;
char buf[LENGTH];
bzero(buf, LENGTH);

sprintf(buf,"%s",command);

	for (i=0;i<nnodes;i++)
	{
		printf("looking at %s\n",nodes[i].type);

		if (strcmp(nodes[i].type,"slave")==0)
		{
			printf("broadcast to %s\n",nodes[i].type);
			if(send(nodes[i].sock, buf, LENGTH, 0) < 0)
			{
				printf("%s\n", strerror(errno));
				return -1;
			}
		}
	}

return 0;
}

struct node_struct* node_find(char *ip)
{
int i;
	for (i=0;i<nnodes;i++)
	{
		if (strcmp(nodes[i].ip,ip)==0)
		{
			return &(nodes[i]);
		}
	}

return NULL;
}

struct node_struct* node_find_master()
{
int i;
	for (i=0;i<nnodes;i++)
	{
		if (strcmp(nodes[i].type,"master")==0)
		{
			return &(nodes[i]);
		}
	}

return NULL;
}


int register_node(int sock,char *node_name)
{
//printf("send register node\n");
char sdbuf[LENGTH]; // Receiver buffer
int cpus;

	//struct ifreq ifr;
	//ifr.ifr_addr.sa_family = AF_INET;
    //strncpy(ifr.ifr_name , interface , IFNAMSIZ-1);
	//ioctl(sock, SIOCGIFADDR, &ifr);

	//sprintf(my_ip,"%s",inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

	cpus = sysconf( _SC_NPROCESSORS_ONLN );

	bzero(sdbuf, LENGTH);

	sprintf(sdbuf,"gpvdmaddnode\n#node_name\n%s\n#cpus\n%d\n#ver\n#1.0\n#end",node_name,cpus);

	if(send(sock, sdbuf, LENGTH, 0) < 0)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}
	
}

int close_all_open()
{
int i;
	for (i=0;i<nnodes;i++)
	{
		if (nodes[i].sock!=-1)
		{
    		close (nodes[i].sock);
		}
	}

}

int send_delete_node(int sock)
{
//printf("send deregister node\n");
char sdbuf[LENGTH]; // Receiver buffer
int cpus;

	cpus = sysconf( _SC_NPROCESSORS_ONLN );

	bzero(sdbuf, LENGTH);

	sprintf(sdbuf,"gpvddeletenode");

	if(send(sock, sdbuf, LENGTH, 0) < 0)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}
	
}

int cmp_addnode(int sock_han,char *revbuf)
{
char my_ip[20];
struct inp_file decode;
char host_name[100];
char full_host_name[100];
int cpus=0;
	if (cmpstr_min(revbuf,"gpvdmaddnode")==0)
	{

		//printf( "address is %s:%u\n",, (unsigned)ntohs(adr_inet.sin_port));
		get_ip_from_sock(my_ip,sock_han);

		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		inp_search_string(&decode,host_name,"#node_name");
		inp_search_int(&decode,&cpus,"#cpus");
		sprintf(full_host_name,"%s%s",my_ip,host_name);
		node_add("slave",my_ip,cpus,sock_han);
		nodes_print();
		return 0;
	}

return -1;
}

int cmp_deletenode(int sock_han,char *revbuf)
{
char my_ip[20];
struct inp_file decode;
char host_name[100];
char full_host_name[100];
int cpus=0;
	if (cmpstr_min(revbuf,"gpvddeletenode")==0)
	{


		//printf( "address is %s:%u\n",, (unsigned)ntohs(adr_inet.sin_port));
		get_ip_from_sock(my_ip,sock_han);
		node_delete(my_ip);
		return 0;
	}

return -1;
}

void nodes_print()
{
int i;
printf("number\tname\tip\t\tcpus\tsock\tload\n");
	for (i=0;i<nnodes;i++)
	{
		printf("%d\t%s\t%s\t%d\t%d\t%d\n",i,nodes[i].type,nodes[i].ip,nodes[i].cpus,nodes[i].sock,nodes[i].load);
	}
}

int cmp_sendnodelist(int sock,char *revbuf)
{
//printf("test send list %s\n",revbuf);
	if (cmpstr_min(revbuf,"gpvdmsendnodelist")==0)
	{
		printf("send list\n");
		nodes_txnodelist();
		return 0;
	}

return -1;
}

int nodes_txnodelist()
{
	char buf[LENGTH];
	char temp[50];
	int i;

	struct node_struct* master=NULL;
	master=node_find_master();

	if (master!=NULL)
	{
		bzero(buf, LENGTH);

		sprintf(buf,"gpvdmnodelist\n");

		if(send(master->sock, buf, LENGTH, 0) < 0)
		{
			printf("%s\n", strerror(errno));
			return -1;
		}

		bzero(buf, LENGTH);


		for (i=0;i<nnodes;i++)
		{
			sprintf(temp,"%s:%d:%d\n",nodes[i].ip,nodes[i].cpus,nodes[i].load);
			strcat(buf,temp);
		}

		if(send(master->sock, buf, LENGTH, 0) < 0)
		{
			printf("%s\n", strerror(errno));
			return -1;
		}
	}

return 0;
}

void nodes_reset()
{
nnodes=0;
}

int node_add(char *type,char *ip,int cpus, int sock)
{
int i;
	for (i=0;i<nnodes;i++)
	{
		if ((strcmp(nodes[i].ip,"none")==0)||(strcmp(nodes[i].type,"master")==0))
		{
				strcpy(nodes[i].ip,ip);
				strcpy(nodes[i].type,type);
				nodes[i].cpus=cpus;
				nodes[i].sock=sock;
				nodes[i].load=0;
				return 0;
		}
	}

	strcpy(nodes[nnodes].ip,ip);
	strcpy(nodes[nnodes].type,type);
	nodes[nnodes].cpus=cpus;
	nodes[nnodes].sock=sock;
	nodes[nnodes].load=0;
	nnodes++;
nodes_print();
return 0;
}

pthread_mutex_t lock;

void copy_dir_to_all_nodes(char *dir)
{
printf("copy_dir_to_all_nodes\n");
int i=0;
		for (i=0;i<nnodes;i++)
		{
			if (strcmp(nodes[i].ip,"none")!=0)
			{
				if (strcmp(nodes[i].type,"slave")==0)
				{
					char full_path[200];

					join_path(2,full_path,calpath_get_store_path(), dir);
					printf("sending dir %s to %s\n",dir,nodes[i].ip);
					printf("Send to %s\n",full_path);
					send_dir(nodes[i].sock,full_path, 0,full_path,dir);

				}
			}
		}
}

void run_jobs(int sock)
{
//printf("run jobs1\n");
pthread_mutex_lock(&lock);
//printf("run jobs2\n");

if (nnodes==0)
{
	printf("Warning no nodes!!!!!!!!!!!\n");
}
int i;
//printf("run jobs3\n");

nodes_print();
printf("here xxx\n");
	struct job* next=NULL;
	do
	{
		next=jobs_get_next();

		if (next==NULL)
		{
			break;
		}

		int sent_job=FALSE;

		for (i=0;i<nnodes;i++)
		{
			if (strcmp(nodes[i].ip,"none")!=0)
			{
				if (strcmp(nodes[i].type,"slave")==0)
				{
					if (nodes[i].load<nodes[i].cpus)
					{
						char full_path[200];
						printf("sending job to %s\n",nodes[i].ip);
						join_path(2,full_path,calpath_get_store_path(), next->name);

						send_dir(nodes[i].sock,full_path, 0,full_path,next->name);
						//send_command(nodes[i].sock,"/home/rod/test/280416/go.o ",next->name,next->cpus_needed);
						send_command(nodes[i].sock,"ls;sleep 30; ll ",next->name,next->cpus_needed);
						strcpy(next->ip,nodes[i].ip);
						nodes[i].load++;
						next->status=1;
						nodes_print();
						jobs_print();
						sent_job=TRUE;
						break;
					}
				}
			}
		}

		if (sent_job==FALSE)
		{
			break;
		}

	}
	while(1);
pthread_mutex_unlock(&lock);
//printf("run jobs4\n");

}

int cmp_simfinished(int sock,char *revbuf)
{
	char dir_name[200];
	int cpus=0;
	char ip[200];
	char buf[512];
	char full_dir[200];
	int ret=0;
	if (cmpstr_min(revbuf,"gpvdmsimfinished")==0)
	{

		struct inp_file decode;
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		inp_search_string(&decode,dir_name,"#dir_name");
		inp_search_int(&decode,&cpus,"#cpus");
		inp_search_string(&decode,ip,"#ip");

		struct job* job=NULL;
		job=jobs_find_job(dir_name);
		if (job!=NULL)
		{

			job->status=2;

			jobs_print();

			run_jobs(sock);

			struct node_struct* node=NULL;

			node=node_find(ip);
			if (node==NULL)
			{
				printf("I can't find IP %s\n",ip);
			}

			node->load-=cpus;
		}



		struct node_struct* master=NULL;
		master=node_find_master();
		//printf("sending @master - want!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		if (master!=NULL)
		{
			struct job* my_job=jobs_find_job(dir_name);
			if (my_job!=NULL)
			{
				//printf("sending @master!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s\n",dir_name);

				join_path(2,full_dir,calpath_get_store_path(), dir_name);

			
				ret=send_dir(master->sock,full_dir, 0,full_dir,my_job->target);

				if (ret!=0)
				{
					printf("dir not found %s %s\n",dir_name, calpath_get_store_path());
				}

			}else
			{
				printf("Can't find job %s\n",dir_name);
				jobs_print();
			}
	printf("here5\n");

			bzero(buf, LENGTH);

			sprintf(buf,"gpvdmpercent\n#percent\n%lf\n#end",jobs_cal_percent_finished());

			if(send(master->sock, buf, LENGTH, 0) < 0)
			{
				printf("%s\n", strerror(errno));
				return -1;
			}

			printf("jobs remaining %d\n",jobs_remaining());
			if (jobs_remaining()==0)
			{
				bzero(buf, LENGTH);

				sprintf(buf,"gpvdmfinished\n");

				if(send(master->sock, buf, LENGTH, 0) < 0)
				{
					printf("%s\n", strerror(errno));
					return -1;
				}

				jobs_clear_all();
			}
	printf("here6\n");


		}

		return 0;
	}

return -1;
}

void node_delete(char *ip)
{
int i;

	for (i=0;i<nnodes;i++)
	{
		//printf("cmp='%s' '%s'\n",ip,nodes[i].ip);
		if (strcmp(nodes[i].ip,ip)==0)
		{
			strcpy(nodes[i].ip,"none");
			strcpy(nodes[i].type,"");
			nodes[i].cpus=-1;
			nodes[i].sock=-1;
			nodes[i].load=0;
			break;
		}
	}

	nodes_print();
}
