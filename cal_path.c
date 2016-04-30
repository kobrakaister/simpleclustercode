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
#include<pthread.h>

static char file_store_path[200];
void calpath_set_store_path(char *in)
{
	strcpy(file_store_path,in);
}

char* calpath_get_store_path()
{
	return file_store_path;
}
