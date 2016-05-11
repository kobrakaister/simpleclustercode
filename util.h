//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.roderickmackenzie.eu
//	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.

#ifndef util_h
#define util_h

#define TRUE 1
#define FALSE 0

#define BACKLOG 10
#define LENGTH 512 // Buffer length

struct job
{
	char name[100];
	int done;
	int status;
	int cpus_needed;
	char target[100];
	char ip[100];
	int copy_state;
};


struct node_struct
{
	char ip[100];
	char type[100];
	char host_name[100];
	int cpus;
	int load;
	int sock;
	double load0;
	int max_cpus;
	time_t alive;
};


int nodes_html_load(char *buf);
void stop_all_jobs();
void calpath_set_exe_name(char *in);
char* calpath_get_exe_name();
int send_all(int sock, void *buffer, int length, int encode);

int cmp_node_send_data(int sock,char *revbuf);
int cmp_get_data(int sock,char *revbuf);

struct node_struct* node_find_master();
struct node_struct* node_find(char *ip);
int cmp_node_runjob(int sock_han,char *revbuf);
int cmp_addnode(int sock_han,char *revbuf);
void nodes_print();
int node_add(char *name,char *ip, int cpus,int sock,char *host_name);

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
int register_node(int sock);
int send_delete_node(int sock);
int cmp_deletenode(int sock_han,char *revbuf);
void node_delete(char *ip);
struct job* jobs_get_next();
int send_command(int sockfd,char *command,char *dir_name,int cpus);
int cmp_simfinished(int sock,char *revbuf);
char* get_my_ip();
int cal_my_ip(int sock);
int get_ip_from_sock(char *out,int sock);
double jobs_cal_percent_finished();
int jobs_remaining();
void jobs_clear_all();
int close_all_open();
int broadcast_to_nodes(char *command);
int cmp_node_killall(int sock,char *revbuf);
int cmp_head_killall(int sock,char *revbuf);
int cmp_node_sleep(int sock,char *revbuf);
int cmp_head_sleep(int sock,char *revbuf);
int cmp_node_poweroff(int sock,char *revbuf);
int cmp_head_poweroff(int sock,char *revbuf);
int cmp_sendnodelist(int sock,char *revbuf);
int nodes_txnodelist();
int cmp_head_exe(int sock,char *revbuf);
void remove_dir(char* dir_name);
int isdir(char *dir);
void copy_dir_to_all_nodes(char *dir);

void jobs_print();
void jobs_reset();
void jobs_add(char *name,char *job_orig_path);
void *rx_loop(void *s);
void run_jobs();
struct job* get_jobs_array();
int get_njobs();
int cmp_register_master(int sock,char *revbuf);
int cmp_master_clean(int sock,char *revbuf);
int cmp_slave_clean(int sock,char *revbuf);

void calpath_set_store_path(char *in);
char* calpath_get_store_path();
int cmp_runjobs(int sock_han,char *revbuf);
struct job* jobs_find_job(char *name);



void textcolor(int color);
void set_porgress_color(int in);
void set_progress_colored();
void set_porgress_nospin();
void set_porgress_noreset();
void set_porgress_max(int in);
void text_progress(double percent);
void progress_clear(int n);
void text_progress_finish();
void set_progress_multi_line_text(char *in);
void set_progress_multi_line();
void text_progress_start(char *in);

int cmp_rxloadstats(int sock,char *revbuf);
int cmp_nodeload(int sock,char *revbuf);
int send_node_load(int sock);
int cmp_node_quit(int sock,char *revbuf);
int cmp_head_quit(int sock,char *revbuf);

void encrypt(char *data,int round_len);
void decrypt(char *data,int round_len);

int cmp_rxsetmaxloads(int sock,char *revbuf);
void encrypt_load();
int node_alive_time(struct node_struct* node);

#endif
