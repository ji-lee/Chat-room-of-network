#ifndef __LINK_H_
#define __LINK_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>

typedef struct
{
	int id;                //存储用户id
	char passwd[20];       //存储用户密码
	int sockfd;            //存储用户套接字，离线时为-1
}PNODE_DATA;


typedef struct node
{
	PNODE_DATA pnode_data;
	struct node * next;
}NODE, *PNODE;

PNODE head;

PNODE create_node(PNODE_DATA pnode_data);                  
void add_node(PNODE head, PNODE_DATA pnode_data);
PNODE find_pre_node(PNODE head, int id); 

PNODE load_data();
void save_data(PNODE head);

#endif
