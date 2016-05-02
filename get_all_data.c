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

int send_command(int sockfd,char *command,char *dir_name,int cpus)
{

    char sdbuf[LENGTH]; // Receiver buffer

	bzero(sdbuf, LENGTH);

	sprintf(sdbuf,"gpvdmcommand\n#command\n%s\n#dir_name\n%s\n#cpus\n%d\n#end",command,dir_name,cpus);

    if(send(sockfd, sdbuf, LENGTH, 0) < 0)
    {
		printf("%s\n", strerror(errno));
	    return -1;
    }

return 0;
}

int cmp_command(int sock,char *revbuf)
{
	char command[200];
	char dir_name[200];
	char buf[LENGTH];
	int cpus=0;
	if (cmpstr_min(revbuf,"gpvdmget_all_data")==0)
	{

		struct inp_file decode;
		//printf("revbuf='%s' %d\n",revbuf,cmpstr_min(revbuf,"gpvdmcommand"));
		inp_init(&decode);
		decode.data=revbuf;
		decode.fsize=strlen(revbuf);
		inp_search_string(&decode,command,"#command");
		inp_search_string(&decode,dir_name,"#dir_name");
		inp_search_int(&decode,&cpus,"#cpus");
		//printf("I will run %s\n",command);
		if (fork()==0)
		{

			char sim_dir[200];
			join_path(2,sim_dir,calpath_get_store_path(), dir_name);
			//printf("change dir to %s\n",sim_dir);
			chdir(sim_dir);

			system(command);
			
			bzero(buf, LENGTH);


			send_dir(sock,sim_dir, 0,calpath_get_store_path(),"");

			sprintf(buf,"gpvdmsimfinished\n#dir_name\n%s\n#cpus\n%d\n#ip\n%s\n#end",dir_name,cpus,get_my_ip());

			if(send(sock, buf, LENGTH, 0) < 0)
			{
				printf("%s\n", strerror(errno));
				return -1;
			}
			 _exit(EXIT_SUCCESS);
		}
	}

return -1;
}

