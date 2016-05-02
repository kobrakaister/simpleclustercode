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

int cmp_node_poweroff(int sock,char *revbuf)
{
	if (cmpstr_min(revbuf,"gpvdmnodepoweroff")==0)
	{
		printf("poweroff\n");
	}

return -1;
}

int cmp_head_poweroff(int sock,char *revbuf)
{
	if (cmpstr_min(revbuf,"gpvdmpoweroff")==0)
	{
		broadcast_to_nodes(sock,"gpvdmnodepoweroff");
	}

return -1;
}

