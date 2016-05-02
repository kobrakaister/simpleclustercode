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
char target[400];

	if (cmpstr_min(revbuf,"gpvdmaddjob")==0)
	{
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		sprintf(job_name,"job%d",njobs);
		inp_search_string(&decode,target,"#target");
		jobs_add(job_name,target);
		jobs_print();
		return 0;
	}

return -1;
}

void jobs_print()
{
int i;

printf("n\tname\tdone\tstatus\ttarget\t\t\t\t\tip\tcopystate\n");

	for (i=0;i<njobs;i++)
	{
		printf("%d\t%s\t%d\t%d\t%s\t%s\t%d\n",i,jobs[i].name,jobs[i].done,jobs[i].status,jobs[i].target,jobs[i].ip,jobs[i].copy_state);
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

void jobs_add(char *name,char *target)
{
	strcpy(jobs[njobs].name,name);
	strcpy(jobs[njobs].target,target);
	strcpy(jobs[njobs].ip,"");
	jobs[njobs].done=FALSE;
	jobs[njobs].status=0;
	jobs[njobs].copy_state=0;
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

double jobs_cal_percent_finished()
{
int i=0;
int finished=0;

	for (i=0;i<njobs;i++)
	{
		if (jobs[i].status==2)
		{
			finished++;
		}
	}
return 100.0*((double)finished)/((double)njobs);
}

struct job* jobs_find_job(char *target)
{
int i=0;
if (strcmp(target,"")==0)
{
	return NULL;
}

	for (i=0;i<njobs;i++)
	{
		if (strcmp(jobs[i].target,target)==0)
		{
			return &(jobs[i]);
		}
	}

	for (i=0;i<njobs;i++)
	{
		if (strcmp(jobs[i].name,target)==0)
		{
			return &(jobs[i]);
		}
	}
return NULL;
}

int jobs_remaining()
{
int i=0;
int left=0;

	for (i=0;i<njobs;i++)
	{
		//printf("%d %d %d\n",i,jobs[i].status,njobs);
		if (jobs[i].status==2)
		{
			left++;
		}
	}
return njobs-left;
}

void jobs_clear_all()
{
njobs=0;
}
