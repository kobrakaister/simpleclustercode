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
#include <sys/wait.h>
#include <sys/time.h>

#include "util.h"
#include "inp.h"

static int global_sock=0;


void log_alarm_wakeup (int i)
{
		int ii;
		FILE *out;
		struct node_struct *nodes=nodes_list();
		struct itimerval tout_val;

		signal(SIGALRM,log_alarm_wakeup);

		tout_val.it_interval.tv_sec = 0;
		tout_val.it_interval.tv_usec = 0;
		tout_val.it_value.tv_sec = 60;
		tout_val.it_value.tv_usec = 0;

		char path[200];
		for (ii=0;ii<nodes_get_nnodes();ii++)
		{
			sprintf(path,"logs/%s",nodes[ii].ip);
			out=fopen(path,"a");
			fprintf(out,"%lf\n",nodes[ii].load0);
			fclose(out);
		}

		setitimer(ITIMER_REAL, &tout_val,0);
}

