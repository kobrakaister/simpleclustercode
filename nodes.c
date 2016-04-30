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

int register_node(int sock,char *node_name)
{
printf("send register node\n");
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

int send_delete_node(int sock)
{
printf("send deregister node\n");
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
printf("cmp add node\n");
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
		node_add(full_host_name,my_ip,cpus,sock_han);
		nodes_print();
		send_file(sock_han,"","node.inp");
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
printf("cmp delete node\n");
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
		printf("%d\t%s\t%s\t%d\t%d\t%d\n",i,nodes[i].name,nodes[i].ip,nodes[i].cpus,nodes[i].sock,nodes[i].load);
	}
}

void nodes_reset()
{
nnodes=0;
}

int node_add(char *name,char *ip,int cpus, int sock)
{
int i;
	for (i=0;i<nnodes;i++)
	{
		if (strcmp(nodes[i].ip,"none")==0)
		{
				strcpy(nodes[i].ip,ip);
				strcpy(nodes[i].name,"name");
				nodes[i].cpus=cpus;
				nodes[i].sock=sock;
				nodes[i].load=0;
				return 0;
		}
	}

	strcpy(nodes[nnodes].ip,ip);
	strcpy(nodes[nnodes].name,"name");
	nodes[nnodes].cpus=cpus;
	nodes[nnodes].sock=sock;
	nodes[nnodes].load=0;
	nnodes++;
nodes_print();
return 0;
}

pthread_mutex_t lock;

void run_jobs(int sock)
{
printf("run jobs1\n");
pthread_mutex_lock(&lock);
printf("run jobs2\n");

if (nnodes==0)
{
	printf("Warning no nodes!!!!!!!!!!!\n");
}
int i;
printf("run jobs3\n");

nodes_print();
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
				if (nodes[i].load<nodes[i].cpus)
				{
					char full_path[200];
					printf("sending job to %s\n",nodes[i].ip);
					join_path(2,full_path,calpath_get_store_path(), next->name);

					send_dir(nodes[i].sock,full_path, 0,calpath_get_store_path());
					send_command(nodes[i].sock,"pwd;ls;sleep 10;md5sum * >sum ",next->name,next->cpus_needed);

					nodes[i].load++;
					next->status=1;
					nodes_print();
					jobs_print();
					sent_job=TRUE;
					break;
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
printf("run jobs4\n");

}

int cmp_simfinished(int sock,char *revbuf)
{
	char dir_name[200];
	int cpus=0;
	char ip[200];
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
			node->load-=cpus;
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
		printf("cmp='%s' '%s'\n",ip,nodes[i].ip);
		if (strcmp(nodes[i].ip,ip)==0)
		{
			strcpy(nodes[i].ip,"none");
			strcpy(nodes[i].name,"");
			nodes[i].cpus=-1;
			nodes[i].sock=-1;
			nodes[i].load=0;
			break;
		}
	}

	nodes_print();
}
