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
#include "tx_packet.h"
#include <time.h>

void get_full_time(char *out,time_t t)
{
	struct tm tm = *localtime(&t);
	if (t!=0)
	{
		sprintf(out,"%d/%d/%d-%d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}else
	{
		strcpy(out,"-");
	}
}

int jobs_load()
{
	FILE * fp;
	char *line;
	size_t len = 0;
	ssize_t read;
	int in_number;
	char in_name[100];
	int in_done;
	int in_status;
	char in_target[200];
	char in_ip[200];
	int in_copy_state;

	struct job *my_jobs=get_jobs_array();
	int n;

	line=malloc(sizeof(char)*500);
	fp = fopen("saved_jobs.dat", "r");
	if (fp == NULL)
	{
		printf("no last data file\n");
		return -1;
	}
	read = getline(&line, &len, fp);

	while ((read = getline(&line, &len, fp)) != -1)
	{
		if (len!=0)
		{
			sscanf(line,"%d%s%d%d%s%s%d", &in_number,in_name,&in_done,&in_status,in_target,in_ip,&in_copy_state);
			//printf("%d %s %d %d %s %s %d\n", in_number,in_name,in_done,&in_status,in_target,in_ip,in_copy_state);
			jobs_add(in_name,in_target);
			n=get_njobs()-1;
			strcpy(my_jobs[n].ip,in_ip);
			my_jobs[n].status=2;
		
		}
	}

	fclose(fp);
	free(line);

char buf[10000];
gen_job_list(buf);
printf("%s\n",buf);
}

void gen_job_list(char *buf)
{
int i;
char temp[1000];
char t_start[100];
char t_stop[100];
struct job* jobs=get_jobs_array();

int njobs=get_njobs();

	sprintf(buf,"n\tname\tdone\tstatus\ttarget\t\t\t\t\tip\tcopystate\tstart\tstop\n");

	for (i=0;i<njobs;i++)
	{
		get_full_time(t_start,jobs[i].t_start);
		//printf("%s\n",t_start);

		get_full_time(t_stop,jobs[i].t_stop);
		//printf("%s\n",t_stop);

		sprintf(temp,"%d\t%s\t%d\t%d\t%s\t%s\t%d\t%s\t%s\n",i,jobs[i].name,jobs[i].done,jobs[i].status,jobs[i].target,jobs[i].ip,jobs[i].copy_state,t_start,t_stop);
		strcat(buf,temp);
		//printf("%s\n",temp);
	}

}

void jobs_print()
{
char buf[10000];
gen_job_list(buf);
printf("%s\n",buf);
}

void jobs_save()
{
char buf[10000];
gen_job_list(buf);
FILE *out=NULL;
out=fopen("saved_jobs.dat","w");
fprintf(out,"%s\n",buf);
fclose(out);
}
