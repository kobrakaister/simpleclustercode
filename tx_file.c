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
#include "util.h"

#include <sys/types.h>
#include <sys/wait.h>

int send_dir(int sockfd,const char *name, int level,char *base_name, char* target)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
	{
        return -1;
	}

    if (!(entry = readdir(dir)))
	{
        return -1;
	}

    do
	{
		if (entry->d_type == DT_DIR)
		{
            char path[1024];
            int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
            path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			{
                continue;
			}
            send_dir(sockfd,path, level + 1,base_name,"");
        }else
        {
			char full_path[500];
			join_path(2,full_path,name, entry->d_name);
			send_file(sockfd,base_name, full_path,target);
			//printf("%s\n", full_path);
		}
    }while (entry = readdir(dir));

    closedir(dir);
return 0;
}

int send_file(int sockfd,char *base_name,char *file_name,char *target)
{
	char rel_name[400];
	char *buf=NULL;
	int packet_size=0;
	strcpy(rel_name,file_name+strlen(base_name));

	struct stat results;
	stat(file_name, &results);

	if (results.st_size>0)
	{
		packet_size=((((int)results.st_size)/((int)LENGTH))+2)*LENGTH;
	}else
	{
		packet_size=LENGTH;
	}

	buf=(char*)malloc(sizeof(char)*packet_size);
	bzero(buf, packet_size);
	//printf("gpvdmfile\n#file_name\n%s\n#file_size\n%d\n#target\n%s\n#end",rel_name,results.st_size,target);
	sprintf(buf,"gpvdmfile\n#file_name\n%s\n#file_size\n%d\n#target\n%s\n#end",rel_name,results.st_size,target);

	if (results.st_size>0)
	{
		int f_block_sz = 0;

		FILE *fp=NULL;

		fp = fopen(file_name, "r");
		if (fp == NULL)
		{
			printf(" %s\n", strerror(errno));
			return -1;
		}

		f_block_sz = fread((buf+LENGTH), sizeof(char), results.st_size, fp);

		if (f_block_sz!=results.st_size)
		{
			printf("Not read enough data\n");
			return -1;		
		}

		fclose(fp);
	}

    if(send(sockfd, buf, packet_size, 0) < 0)
    {
		printf("%s\n", strerror(errno));
	    return -1;
    }

	free(buf);

return 0;
}
