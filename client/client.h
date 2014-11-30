#ifndef	__CLIENT_H_
#define	__CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<strings.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<pthread.h>
#include<ctype.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<time.h>
#include<dirent.h>
#include<errno.h>


#define SERV_PORT	9527
#define MAXLINE		4096
//#define STRLEN		30

typedef struct
{
	char com[30];                        //存储协议命令
	int id;                              //用户id
	char message[MAXLINE];               //存储消息
	int id_name;                         //备用，比如在进行私聊时用来保存对方用户id, 
}MESSAGE;

		
char time_mes[MAXLINE];
char save_message[100];


void mygets(char buf[], int len);
void press_any_one();

void *sel(void *arg);
void *next_select(void *arg);
void *send_pri_mes(void *arg);
void *send_group_mes(void *arg);

void login();
void regist();
void show_menu_first();
void quit(int );

void show_menu_second();
void private_chat();
void view_online_id();
void group_chat();
void view_chat_history();
void view_private_chat_history(int);
void view_group_chat_history();

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);


int 		sockfd;
int 		my_id;
int			chat_id;
time_t		ticks;
pthread_t	tid;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;              //线程锁

#endif
