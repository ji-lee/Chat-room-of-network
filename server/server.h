#ifndef	__SERVER_H_
#define __SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<strings.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<string.h>
#include<pthread.h>

#define SERV_PORT	9527
#define MAXLINE		4096


//发送给客户端选择结果
char	buf0[MAXLINE] = "login success";
char	buf1[MAXLINE] = "login faile";
char	buf3[MAXLINE] = "regist success";
char	buf4[MAXLINE] = "regist faile";
char	buf5[MAXLINE] = "private_chat success";
char	buf6[MAXLINE] = "private_chat faile";
char	buf7[MAXLINE] = "the id has already logined";
char  	buf8[MAXLINE] = "group chat success";

typedef struct
{
	char com[30];                  //存储协议命令
	int id;                        //用户id
	char message[MAXLINE];         //存储消息
	int id_name;                   //备用，比如在进行私聊时用来保存对方的id
}DATA;



int	listenfd;
int connfd[100] = {-1};
int find_sockfd[50];               //用于接收每个客户端的连接套接字，每当有客户端退出时，该数组里相应的位回初始化为0
pthread_t tid[50];                 //注意，服务器只能打开50个(不一定是同时，而是累计)线程
int group_chat_sockfd[50];

void *str_echo(void *param);


ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  //线程锁

#endif
