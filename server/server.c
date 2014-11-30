#include"server.h"
#include"link.h"


//注：服务器所有的打印消息均用于方便程序的跟踪调试


/****************************************************
*	函数介绍：主函数								*
*	实现功能：建立套接字链接						*
*	输入参数：无									*	
*	返回值  ：0 									*
****************************************************/
int main(int argc, char **argv)
{
	head = load_data();
	PNODE p = head->next;
	
	while(p != NULL)
	{
		printf("id = %d, sockfd = %d\n", p->pnode_data.id, p->pnode_data.sockfd);
		p = p->next;
	}
	
	socklen_t	clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("listenfd = %d\n", listenfd);

	bzero(&servaddr, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	listen(listenfd, 5);

	printf("Now listening port %d\n", SERV_PORT);

	int times = 0;
	for( ; ; )
	{
		clilen = sizeof(cliaddr);

		connfd[times] = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
		printf("connfd[%d] = %d\n", times, connfd[times]);

		int ret;
		ret = pthread_create(&tid[times], NULL, str_echo, &connfd[times]);
		if(ret != 0)
		{
			perror("pthread_create");
			return -1;
		}
		else
		{
			printf("ip: %s connected...\n", inet_ntoa(cliaddr.sin_addr));
		}
		
		times++;
	}
	
	return 0;
}

/****************************************************
*	函数介绍：接收客户端消息						*
*	实现功能：处理客户端发来的消息					*
*	输入参数：客户端连接套接字						*	
*	返回值  ：无									*
****************************************************/
void *str_echo(void *param)
{
	int sockfd = *(int *)param;
	int my_id;
	int find_id;
//	ssize_t n;
	DATA data;
	
	printf("sockfd = %d\n", sockfd);

	int num = 0;
	while(readn(sockfd, &data, sizeof(DATA)) > 0)
	{	
		num++;
		printf("\ncom[%d]: %s\n", num, data.com);

		if(strcmp(data.com, "exit") == 0)
		{
			printf("^_^ id为%d的用户已经退出.\n", data.id);
			PNODE find = find_pre_node(head, data.id);
			
			printf("find->next->pnode_data.sockfd = %d\n", find->next->pnode_data.sockfd);

			int i;
			for(i = 0; i < 50; i++)
			{
				if(find_sockfd[i] == find->next->pnode_data.sockfd)
				{
					pthread_mutex_lock(&mutex);                         //线程锁，防止其他线程同时更改全局变量find_sockfd[]
					find_sockfd[i] = 0;
					printf("find_sockfd[%d]初始化为0.\n", i);	
					pthread_mutex_unlock(&mutex);                       //解锁
				}
			}
			
			find->next->pnode_data.sockfd = -1;
			printf("^_^用户%d的链表sockfd已初始化为%d\n", data.id, find->next->pnode_data.sockfd);
		}
		
		else if(strcmp(data.com, "login") == 0)                         //登录      
		{
			PNODE find = find_pre_node(head, data.id);            
			if(NULL == find || (strcmp(find->next->pnode_data.passwd, data.message) != 0))    //登录失败
			{
				strcpy(data.message, buf1);
				writen(sockfd, &data, sizeof(DATA));
			}
			else if(find->next->pnode_data.sockfd != -1)                //id已经登录 
			{
				strcpy(data.message, buf7);
				writen(sockfd, &data, sizeof(DATA));
			}
			else                                                        //登录成功
			{
				my_id = find->next->pnode_data.id;

				printf("登录前sockfd = %d\n", find->next->pnode_data.sockfd);
				printf("id为%d的用户登录成功.\n", find->next->pnode_data.id);
				find->next->pnode_data.sockfd = sockfd;
				printf("登录后sockfd = %d\n", find->next->pnode_data.sockfd);
				
				int i;
				for(i = 0; i < 50; i++)
				{
					if(find_sockfd[i])
					{
						char online_message[MAXLINE];
						char online_id_str[5];

						sprintf(online_id_str, "%d", find->next->pnode_data.id);
						strcpy(online_message, "\nUser ");
						strcat(online_message, online_id_str);
						strcat(online_message, " is online now.");

						strcpy(data.com, "user_online");
						strcpy(data.message, online_message);
						
						printf("发送上线提醒到连接套接字为%d的在线用户.\n", find_sockfd[i]);
						writen(find_sockfd[i], &data, sizeof(DATA));
					}
				}
				
				for(i = 0; i < 50; i++)                                 //登录成功，把连接套接字加入到find_sockfd[10]
				{
					if(find_sockfd[i])
						continue;
					
					pthread_mutex_lock(&mutex);
					find_sockfd[i] = find->next->pnode_data.sockfd;
					pthread_mutex_unlock(&mutex);
					
					break;
				}
				
				strcpy(data.message, buf0);
				writen(sockfd, &data, sizeof(DATA));
	
			}
	
		}
		
		else if(strcmp(data.com, "regist") == 0)                        //注册 
		{
			PNODE find = find_pre_node(head, data.id);
			if(NULL == find)                                            //可以注册
			{
				printf("可以创建新用户.\n");
				
				PNODE_DATA pnode_data;
				pnode_data.id = data.id;
				
				my_id = data.id;
				
				strncpy(pnode_data.passwd, data.message, MAXLINE);    
				pnode_data.sockfd = sockfd;
				
				add_node(head, pnode_data);
				
				int i;
				for(i = 0; i < 50; i++)
				{
					if(find_sockfd[i])
					{
						char online_message[MAXLINE];
						char online_id_str[5];

						sprintf(online_id_str, "%d", data.id);
						strcpy(online_message, "\nUser ");
						strcat(online_message, online_id_str);
						strcat(online_message, " is online now.");

						strcpy(data.com, "user_online");
						strcpy(data.message, online_message);
						
						printf("发送上线提醒到连接套接字为%d的在线用户.\n", find_sockfd[i]);
						writen(find_sockfd[i], &data, sizeof(DATA));
					}
				}
				
				for(i = 0; i < 50; i++)                                 //注册成功，把连接套接字加入到find_sockfd[10]
				{
					if(find_sockfd[i])
						continue;
					
					pthread_mutex_lock(&mutex);
					find_sockfd[i] = sockfd;
					pthread_mutex_unlock(&mutex);
					
					break;
				}
				
				strcpy(data.message, buf3);
				writen(sockfd, &data, sizeof(DATA));
			}
			else                                                        //id已经注册
			{
				printf("已经有id存在.\n");
				strcpy(data.message, buf4);
				writen(sockfd, &data, sizeof(DATA));
			}
		}
		
		else if(strcmp(data.com, "private_chat") == 0)                  //私聊
		{
			PNODE find = find_pre_node(head, data.id);
			if(NULL == find || find->next->pnode_data.sockfd == -1)     //没有私聊对象或对象没在线
			{	
				printf("没有要私聊的用户.\n");
				strcpy(data.message, buf6);
				writen(sockfd, &data, sizeof(DATA));
			}
			else                                                        //返回可以私聊的信息给客户端
			{
				find_id = find->next->pnode_data.id;
				data.id = find->next->pnode_data.sockfd;

				printf("可以私聊.\n");
				strcpy(data.message, buf5);
				writen(sockfd, &data, sizeof(DATA));
			}
		}
	
		else if(strcmp(data.com, "pri_data") == 0)                      //发送私聊消息
		{
			if(strcmp(data.message, "exit") == 0)
			{
				printf("用户%d已退出.\n", my_id);
			}
			
			printf("发送消息给%d.\n", find_id);
			data.id_name = my_id;
			writen(data.id, &data, sizeof(DATA));
		}
		
		else if(strcmp(data.com, "group_chat") == 0)                    //把套接字加入到群聊结构体
		{
			int i;
			for(i = 0; i < 50; i++)
			{
				printf("^_^ group_chat_sockfd[%d] = %d, sockfd = %d\n", i, group_chat_sockfd[i], sockfd);

				if(group_chat_sockfd[i] == sockfd)
					break;
				if(group_chat_sockfd[i])
					continue;

				pthread_mutex_lock(&mutex);
				group_chat_sockfd[i] = sockfd;
				pthread_mutex_unlock(&mutex);
					
				break;
			}
			
			printf("把套接字%d加入到group_chat_sockfd成功.\n", sockfd);

			strcpy(data.message, buf8);
			writen(sockfd, &data, sizeof(DATA));
		}
		
		else if(strcmp(data.com, "group_message") == 0)                 //发送群聊消息
		{
			if(strcmp(data.message, "exit") == 0)
			{
				printf("用户%d已退出群聊.\n",my_id);

				int i;
				for(i = 0; i < 50; i++)
				{
					if(group_chat_sockfd[i] == sockfd)
					{
						pthread_mutex_lock(&mutex);
						group_chat_sockfd[i] = 0;
						printf("group_chat_sockfd[%d]初始化为0.\n", i);
						pthread_mutex_unlock(&mutex);
						
						break;
					}
				}
			}
			
			int i;
			for(i = 0; i < 50; i++)
			{
				if(group_chat_sockfd[i] != 0)                           //如果这里的0是-1,write时终端会有信心显示，为什么？                     
				{
					printf("group_chat_sockfd[%d] = %d, my sockfd = %d\n", i, find_sockfd[i], sockfd);

					if((group_chat_sockfd[i] == sockfd) && (strcmp(data.message, "exit") == 0))  //如果是退出群聊的用户就不发送消息
						continue;
					data.id_name = my_id;
					
					printf("发送消息给sockfd为%d的用户...\n", group_chat_sockfd[i]);
					writen(group_chat_sockfd[i], &data, sizeof(DATA));
				}
			}
		}
		
		else if(strcmp(data.com, "view_online_id") == 0)                //发送在线用户列表
		{
			printf("发送在线用户列表.\n");

			strcpy(data.message, "\nNow online user:\n");
			PNODE p = head->next;
			char buf[10];
			while(p != NULL)                                            //遍历链表查看在线用户
			{
				if(p->pnode_data.sockfd != -1)
				{
					sprintf(buf, "%d", p->pnode_data.id);
					strcat(data.message, buf);
					strcat(data.message, "\n");
				}
				p = p->next;
			}
			
			writen(sockfd, &data, sizeof(DATA));
			printf("在线用户列表已发送.\n");
		}
		
		else 
		{
			printf("客户端发来不明消息，忽略.\n");
			continue;
		}
	}
	
	save_data(head);  //保存数据到链表
	close(sockfd);    //关闭连接套接字
	return(NULL);
}

/****************************************************
*	函数介绍：读取客户端消息函数    				*
*	实现功能：替代系统函数read						*
*	输入参数：套接字、字符串、长度					*	
*	返回值  ：字符串长度							*
****************************************************/
ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;

	while(nleft > 0)
	{
		if((nread = read(fd, ptr, nleft)) < 0)
		{
			if(errno == EINTR)
				nread = 0;        //and call read() again
			else
				return(-1);
		}
		else if(nread == 0)       //EOF
			break;
		nleft	-= nread;
		ptr 	+= nread;
	}
	return(n-nleft);             //return >= 0
}

/****************************************************
*	函数介绍：发送消息给客户端函数    				*
*	实现功能：替代系统函数write						*
*	输入参数：套接字、字符串、长度					*	
*	返回值  ：字符串长度							*
****************************************************/
ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t	nleft; 
	ssize_t	nwritten;
	const char	*ptr;

	ptr = vptr; 
	nleft = n;

	while(nleft > 0)
	{
		if((nwritten = write(fd, ptr,nleft)) <= 0)
		{
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return (-1);
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

