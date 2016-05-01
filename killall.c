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

int cmp_node_killall(int sock,char *revbuf)
{
	if (cmpstr_min(revbuf,"gpvdmnodekillall")==0)
	{
		printf("killall go.o\n");
		system("killall go.o");
	}

return -1;
}

int cmp_head_killall(int sock,char *revbuf)
{
	if (cmpstr_min(revbuf,"gpvdmkillall")==0)
	{
		broadcast_to_nodes(sock,"gpvdmnodekillall");
	}

return -1;
}

