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

struct job jobs[1000];
static int njobs=0;

int cmp_addjob(int sock_han,char *revbuf)
{
struct inp_file decode;
char job_name[400];
printf("job compare\n");
	if (cmpstr_min(revbuf,"gpvdmaddjob")==0)
	{
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		inp_search_string(&decode,job_name,"#job_name");
		jobs_add(job_name);
		jobs_print();
		return 0;
	}

return -1;
}

void jobs_print()
{
int i;

printf("n\tname\tdone\tstatus\n");

	for (i=0;i<njobs;i++)
	{
		printf("%d\t%s\t%d\t%d\n",i,jobs[i].name,jobs[i].done,jobs[i].status);
	}
}

void jobs_reset()
{
njobs=0;
}

struct job* jobs_get_next()
{
int i;

	for (i=0;i<njobs;i++)
	{
		if (jobs[i].status==0)
		{
			return &(jobs[i]);
		}
	}
return NULL;
}

void jobs_add(char *name)
{
	strcpy(jobs[njobs].name,name);
	jobs[njobs].done=FALSE;
	jobs[njobs].status=0;
	jobs[njobs].cpus_needed=1;
	njobs++;
}

struct job* get_jobs_array()
{
	return jobs;
}

int get_njobs()
{
	return njobs;
}

struct job* jobs_find_job(char *name)
{
int i=0;

	for (i=0;i<njobs;i++)
	{
		if (strcmp(jobs[i].name,name)==0)
		{
			return &(jobs[i]);
		}
	}
return NULL;
}
