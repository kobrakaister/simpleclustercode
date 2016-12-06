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
#include "inp.h"
#include<pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct node_struct nodes[100];
static int nnodes=0;

struct node_struct *nodes_list()
{
return nodes;
}

int broadcast_to_nodes(struct tx_struct *packet)
{
printf("broadcast\n");
int i;
char buf[LENGTH];

	for (i=0;i<nnodes;i++)
	{

		if (strcmp(nodes[i].type,"slave")==0)
		{
			printf("broadcast to %s\n",nodes[i].host_name);

			tx_packet(nodes[i].sock,packet,NULL);

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


int register_node(int sock)
{

int cpus;
	char host_name[200];

	gethostname(host_name, 200);

	cpus = sysconf( _SC_NPROCESSORS_ONLN );

	struct tx_struct packet;
	tx_struct_init(&packet);
	tx_set_id(&packet,"gpvdmaddnode");
	strcpy(packet.ip, get_my_ip());
	packet.cpus=cpus;
	strcpy(packet.host_name, host_name);

	tx_packet(sock,&packet,NULL);

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
	struct tx_struct packet;
	tx_struct_init(&packet);
	tx_set_id(&packet,"gpvdmdeletenode");

	tx_packet(sock,&packet,NULL);
}

int cmp_addnode(int sock,struct tx_struct *data)
{
struct inp_file decode;
char host_name[100];
int cpus=0;
int ret=0;
	if (cmpstr_min(data->id,"gpvdmaddnode")==0)
	{

		node_add("slave",data->ip,data->cpus,sock,data->host_name);
		nodes_print();
		return 0;
	}

return -1;
}

int cmp_deletenode(int sock_han,struct tx_struct *data)
{
char my_ip[20];
struct inp_file decode;
char host_name[100];
char full_host_name[100];
int cpus=0;
	if (cmpstr_min(data->id,"gpvdmdeletenode")==0)
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
printf("number\ttype\tname\t\tip\t\tcpus\tmax_cpus\tsock\tload\tload0\tlast_seen\n");
	for (i=0;i<nnodes;i++)
	{
		printf("%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%lf\n",i,nodes[i].type,nodes[i].host_name,nodes[i].ip,nodes[i].cpus,nodes[i].max_cpus,nodes[i].sock,nodes[i].load,nodes[i].load0,node_alive_time(&(nodes[i])));
	}
}

int cmp_sendnodelist(int sock,struct tx_struct *data)
{
char buf[LENGTH];
int i;

	if (cmpstr_min(data->id,"gpvdmsendnodelist")==0)
	{
		nodes_txnodelist();
		return 0;
	}

return -1;
}


int nodes_html_load(char *buf)
{

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	int i;
	char temp[200];

	strcpy(buf,"HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n<!DOCTYPE html><html><head>");
	strcat(buf,"<title>Cluster status</title>");
	strcat(buf,"<meta http-equiv=\"refresh\" content=\"1\">");
	strcat(buf,"<body>");
	sprintf(temp,"<h1>Cluster status %d-%d-%d %d:%d:%d:</h1>", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	strcat(buf,temp);

	for (i=0;i<nnodes;i++)
	{
		sprintf(temp,"Node <b>%s</b> (%s CPUs=%d): <b>%lf</b><br>",nodes[i].host_name,nodes[i].ip,nodes[i].cpus,nodes[i].load0);
		strcat(buf,temp);
	}

	strcat(buf,"</body></html>\r\n");

return 0;
}

int nodes_txnodelist()
{
	char *buf=NULL;
	char temp[100];
	int i;

	struct tx_struct packet;
	struct node_struct* master=NULL;
	master=node_find_master();

	if (master!=NULL)
	{
		
		buf=malloc(LENGTH*sizeof(char));
		bzero(buf, LENGTH);

		for (i=0;i<nnodes;i++)
		{
			sprintf(temp,"%s:%s:%d:%d:%lf:%d:%d\n",nodes[i].host_name,nodes[i].ip,nodes[i].cpus,nodes[i].load,nodes[i].load0,nodes[i].max_cpus,node_alive_time(&(nodes[i])));
			strcat(buf,temp);
		}

		tx_struct_init(&packet);
		tx_set_id(&packet,"gpvdmnodelist");
		packet.size=strlen(buf);

		tx_packet(master->sock,&packet,buf);

		free(buf);

	}


return 0;
}

int nodes_get_nnodes()
{
return nnodes;
}

void nodes_reset()
{
nnodes=0;
}

int node_alive_time(struct node_struct* node)
{
return (int)(time(NULL)-node->alive);
}

int node_add(char *type,char *ip,int cpus, int sock,char *host_name)
{
int i;
	for (i=0;i<nnodes;i++)
	{
		if ((strcmp(nodes[i].ip,"none")==0)||(strcmp(nodes[i].type,"master")==0))
		{
				strcpy(nodes[i].ip,ip);
				strcpy(nodes[i].type,type);
				strcpy(nodes[i].host_name,host_name);
				nodes[i].cpus=cpus;
				nodes[i].max_cpus=cpus;
				nodes[i].sock=sock;
				nodes[i].load=0;
				nodes[i].load0=0;
				return 0;
		}
	}

	strcpy(nodes[nnodes].ip,ip);
	strcpy(nodes[nnodes].type,type);
	strcpy(nodes[nnodes].host_name,host_name);
	nodes[nnodes].cpus=cpus;
	nodes[nnodes].max_cpus=cpus;
	nodes[nnodes].sock=sock;
	nodes[nnodes].load=0;
	nodes[nnodes].load0=0;
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
				if ((strcmp(nodes[i].type,"slave")==0)&&(nodes[i].max_cpus!=0))
				{
					char full_path[200];

					join_path(2,full_path,calpath_get_store_path(), dir);
					printf("sending dir %s to %s\n",dir,nodes[i].ip);
					printf("Send to %s\n",full_path);
					send_dir(nodes[i].sock,full_path, 0,full_path,dir);
					//tx_sync_packet_one(nodes[i].sock,full_path, dir);

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
					if (nodes[i].load<nodes[i].max_cpus)
					{
						char full_path[200];
						printf("sending job to %s\n",nodes[i].ip);
						join_path(2,full_path,calpath_get_store_path(), next->name);

						//tx_sync_packet_one(nodes[i].sock,full_path, next->name);
						send_dir(nodes[i].sock,full_path, 0,full_path,next->name);

						send_command(nodes[i].sock,calpath_get_exe_name(),next->name,next->cpus_needed);
						strcpy(next->ip,nodes[i].ip);
						next->t_start=time(NULL);
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

int cmp_simfinished(int sock,struct tx_struct *data)
{
	char buf[512];
	int ret=0;
	struct tx_struct packet;
	if (cmpstr_min(data->id,"gpvdmsimfinished")==0)
	{
		return 0;
		struct job* job=NULL;
		job=jobs_find_job(data->dir_name);
		if (job!=NULL)
		{

			job->status=2;
			job->t_stop=time(NULL);
			jobs_print();

			run_jobs(sock);

			struct node_struct* node=NULL;

			node=node_find(data->ip);
			if (node==NULL)
			{
				printf("I can't find IP %s\n",data->ip);
			}

			printf("load was %d %s \n",node->load,data->dir_name);
			node->load-=data->cpus;
			printf("load now %d %s\n",node->load,data->dir_name);
			jobs_print();
		}



		struct node_struct* master=NULL;
		master=node_find_master();
		//printf("sending @master - want!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		if (master!=NULL)
		{
			/*struct job* my_job=jobs_find_job(data->dir_name);
			if (my_job!=NULL)
			{
				//printf("sending @master!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s\n",data->dir_name);

				join_path(2,full_dir,calpath_get_store_path(), data->dir_name);

			
				ret=send_dir(master->sock,full_dir, 0,full_dir,my_job->target);

				if (ret!=0)
				{
					printf("dir not found %s %s\n",data->dir_name, calpath_get_store_path());
				}

			}else
			{
				printf("Can't find job %s\n",data->dir_name);
				jobs_print();
			}*/
	printf("here5\n");


			tx_struct_init(&packet);
			tx_set_id(&packet,"gpvdmpercent");
			packet.percent=(int)jobs_cal_percent_finished();

			tx_packet(sock,&packet,NULL);


			printf("jobs remaining %d\n",jobs_remaining());
			if (jobs_remaining()==0)
			{

				tx_struct_init(&packet);
				tx_set_id(&packet,"gpvdmfinished");
				tx_packet(sock,&packet,NULL);

				//jobs_clear_all();
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
			nodes[i].load=-1;
			nodes[i].load0=-1;
			break;
		}
	}

	nodes_print();
}
