#ifndef tx_packet_h
#define tx_packet_h
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
#include <sys/stat.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "tx_packet.h"
#include "zlib.h"

struct tx_struct
{
char id[100];
char src[100];
char file_name[100];
int size;
char target[100];
char message[200];
double load0;
double load1;
double load2;
char ip[40];
int stat;
int zip;
char *data;
int uzipsize;
};

void tx_struct_init(struct tx_struct *in);
void tx_set_id(struct tx_struct *in,char *id);
void tx_set_size(struct tx_struct *in,int size);
int tx_packet(int sock,struct tx_struct *in,char *buf);
void tx_set_file_name(struct tx_struct *in,char *file_name);
void tx_set_target(struct tx_struct *in,char *target);
void tx_set_stat(struct tx_struct *in,int stat_in);
int rx_packet(int sock,struct tx_struct *in,char *buf);
void tx_set_src(struct tx_struct *in,char *src);
void tx_set_zip(struct tx_struct *in,int zip);
void tx_set_uzipsize(struct tx_struct *in,int uzipsize);
#endif
