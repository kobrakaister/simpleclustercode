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
#include "tx_packet.h"

int cmp_node_poweroff(int sock,struct tx_struct *data)
{
	if (cmpstr_min(data->id,"gpvdmnodepoweroff")==0)
	{
		printf("poweroff\n");
	}

return -1;
}

int cmp_head_poweroff(int sock,struct tx_struct *data)
{
	if (cmpstr_min(data->id,"gpvdmpoweroff")==0)
	{
		struct tx_struct packet;
		tx_struct_init(&packet);
		tx_set_id(&packet,"gpvdmnodepoweroff");
		broadcast_to_nodes(&packet);
	}

return -1;
}

