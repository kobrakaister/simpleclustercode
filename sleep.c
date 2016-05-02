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

int cmp_node_sleep(int sock,char *revbuf)
{
	if (cmpstr_min(revbuf,"gpvdmnodesleep")==0)
	{
		printf("sleep\n");
	}

return -1;
}

int cmp_head_sleep(int sock,char *revbuf)
{
	if (cmpstr_min(revbuf,"gpvdmsleep")==0)
	{
		broadcast_to_nodes(sock,"gpvdmnodesleep");
	}

return -1;
}

