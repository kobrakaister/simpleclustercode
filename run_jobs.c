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

struct node_struct nodes[100];
static int nnodes=0;

int cmp_runjobs(int sock_han,char *revbuf)
{
	printf("runjobs..\n");
	char command[200];
	if (cmpstr_min(revbuf,"gpvdmrunjobs")==0)
	{
	printf("now runjobs..\n");

		run_jobs();

	}

return -1;
}

