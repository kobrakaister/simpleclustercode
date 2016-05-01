#ifndef util_h
#define util_h

#define TRUE 1
#define FALSE 0

#define BACKLOG 10
#define LENGTH 512 // Buffer length
#define PORT 50002

struct job
{
	char name[100];
	int done;
	int status;
	int cpus_needed;
	char target[100];
};


struct node_struct
{
	char ip[100];
	char type[100];
	int cpus;
	int load;
	int sock;
};

struct node_struct* node_find_master();
int cmp_command(int sock_han,char *revbuf);
int cmp_addnode(int sock_han,char *revbuf);
void nodes_print();
int node_add(char *name,char *ip, int cpus,int sock);

int send_file(int sockfd,char *base_name,char *file_name,char *target);
void mkdirs(char *dir);
int cmpstr_min(char * in1,char *in2);
int english_to_bin( char * in);
int ewe( const char *format, ...);
char *get_file_name_from_path(char *in);
int get_dir_name_from_path(char *out, char *in);
void join_path(int max, ...);
int head();
int node();
int cmp_rxfile(int sock_han,char *revbuf);
int file_rx_and_save(char *file_name,int sock_han,int size);
int send_dir(int sockfd,const char *name, int level,char *base_name,char *target);
int cmp_addjob(int sock_han,char *revbuf);
int register_node(int sock, char *node_name);
int send_delete_node(int sock);
int cmp_deletenode(int sock_han,char *revbuf);
void node_delete(char *ip);
struct job* jobs_get_next();
int send_command(int sockfd,char *command,char *dir_name,int cpus);
int cmp_simfinished(int sock,char *revbuf);
char* get_my_ip();
int cal_my_ip(char *interface);
int get_ip_from_sock(char *out,int sock);
double jobs_cal_percent_finished();
int jobs_remaining();
void jobs_clear_all();
int close_all_open();
int broadcast_to_nodes(int sock,char *command);
int cmp_node_killall(int sock,char *revbuf);
int cmp_head_killall(int sock,char *revbuf);


void jobs_print();
void jobs_reset();
void jobs_add(char *name,char *job_orig_path);
void *rx_loop(void *s);
void run_jobs();
struct job* get_jobs_array();
int get_njobs();
int cmp_register_master(int sock,char *revbuf);
struct job* jobs_find_job_from_target(char *target);

void calpath_set_store_path(char *in);
char* calpath_get_store_path();
int cmp_runjobs(int sock_han,char *revbuf);
struct job* jobs_find_job(char *name);

#endif
