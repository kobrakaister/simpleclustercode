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


int cmp_register_master(int sock,char *revbuf)
{
	char command[200];
	char my_ip[200];
	char buf[LENGTH];
	struct inp_file decode;
	if (cmpstr_min(revbuf,"gpvdmregistermaster")==0)
	{

		//printf( "address is %s:%u\n",, (unsigned)ntohs(adr_inet.sin_port));
		get_ip_from_sock(my_ip,sock);

		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		node_add("master",my_ip,0,sock);
		strcpy(buf,"hello!!!!!\n");
		send(sock, buf, LENGTH, 0);
		return 0;
	}

return -1;
}

