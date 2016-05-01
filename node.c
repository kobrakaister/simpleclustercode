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

#include "util.h"
#include "inp.h"

static int global_sock=0;


void my_handler(int s)
{
	send_delete_node(global_sock);
	printf("Caught signal %d\n",s);
	exit(1); 
}

int node()
{

char node_name[100];
char store_path[200];
char temp_path[200];
char interface[200];

struct inp_file inp;
inp_init(&inp);
if (inp_load(&inp,"node.inp")!=0)
{
	printf("can't find file config.inp");
	exit(0);
}
inp_check(&inp,1.0);
inp_search_string(&inp,node_name,"#node_name");
inp_search_string(&inp,temp_path,"#store_path");
inp_search_string(&inp,interface,"#interface");
inp_free(&inp);
realpath(temp_path, store_path);
calpath_set_store_path(store_path);
cal_my_ip(interface);

    int sockfd; // Socket file descriptor
    int sin_size; // to store struct size
    int nsockfd; // New Socket file descriptor
    struct sockaddr_in addr_remote;
    struct sockaddr_in remote_addr; 
    /* Get the Socket file descriptor */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
		printf(" %s\n", strerror(errno));
        return (0);
    }
    /* Fill the socket address struct */
    remote_addr.sin_family = AF_INET; 
    remote_addr.sin_port = htons(PORT); 
    inet_pton(AF_INET, "127.0.0.1", &remote_addr.sin_addr); 
    bzero(&(remote_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
    {
		printf(" %s\n", strerror(errno));
        return (0);
    }
    else
	{
		printf("client: Connected to server at port %d...ok!\n", PORT);
	}


	char revbuf[LENGTH];
	int f_block_sz;

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	global_sock=sockfd;

	register_node(sockfd,node_name);

	while(f_block_sz = recv(sockfd, revbuf, LENGTH, 0))
	{

		//printf("%s\n",revbuf);

		cmp_command(sockfd,revbuf);

		cmp_rxfile(sockfd,revbuf);

		cmp_node_killall(sockfd,revbuf);

		if(f_block_sz < 0)
		{
			printf(" %s\n", strerror(errno));
			return (0);
		}

		
	}
/////////////////////////////////////////


    close (sockfd);
    printf("client: connection lost.\n");
    return (0);
}
