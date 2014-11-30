#include"client.h"

 
/****************************************************
*	函数介绍：客户端主函数							*
*	实现功能：建立套接字连接、接收服务器消息		*
*	输入参数：服务器ip地址							*	
*	返回值  ：0 									*
****************************************************/
int main(int argc, char **argv)
{
	struct sockaddr_in	servaddr;
	
	if(argc != 2)
	{
		printf("usage: tcpcli<IP address>\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd == -1)
	{
		printf("sockfd open error.\n");
		return 0;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
	{
		printf("connect error.\n");
		return 0;
	}

	//write
	pthread_create(&tid, NULL, sel, NULL);

	MESSAGE mes;     //用于接收服务器结构体消息
	
	//read
	while(readn(sockfd, &mes, sizeof(MESSAGE)) > 0)
	{
		if(strcmp(mes.message, "login success") == 0)                   //登录成功
		{
			my_id = mes.id;                                             //保存自己的连接套接字到全局变量
			
			pthread_create(&tid, NULL, next_select, &my_id);            //进入第二层菜单
		}
		
		else if(strcmp(mes.message, "login faile") == 0)                //登录失败
		{
			system("clear");
			printf("\nThe passwd or id input error. Login failed.\n");
			press_any_one();
			pthread_create(&tid, NULL, sel, NULL);                      //重新返回到登录界面
		}
		
		else if(strcmp(mes.message, "the id has already logined") == 0) //id已经登录
		{
			system("clear");
			printf("\nThe id %d has already logined.\n", mes.id);
			exit(0);
		}
		
		else if(strcmp(mes.message, "regist success") == 0)             //注册成功
		{
			my_id = mes.id;
			
			pthread_create(&tid, NULL, next_select, &my_id);            //进入第二层菜单  
		}
		
		else if(strcmp(mes.message, "regist faile") == 0)               //注册失败
		{
			system("clear");
			printf("\nThe id you inputed had already registed. Regist failed\n");
			
			press_any_one();
			pthread_create(&tid, NULL, sel, NULL);
		}
		
		else if(strcmp(mes.message, "private_chat success") == 0)       //私聊连接成功
		{
			system("clear");
			printf("\nConnecting success. Begin private chat...\n");
			printf("(Input \"exit\" if you want to quit.)\n\n");
			printf("User %d\t\t\t\tUser %d \n", my_id, chat_id);

			pthread_create(&tid, NULL, send_pri_mes, &mes.id);          //私聊发送消息
		}
		
		else if(strcmp(mes.message, "private_chat faile") == 0)         //私聊连接失败
		{
			system("clear");
			printf("\nThe id you inputed no exist or is offline.\n");
			
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);                
		}
		
		else if(strcmp(mes.com, "pri_data") == 0)                       //接收私聊信息并显示
		{
			char my_id_str[10];
			char chat_id_str[10];
			char file_name[20];
	
			sprintf(my_id_str, "%d", my_id);
			sprintf(chat_id_str, "%d", chat_id);
	
			strcpy(file_name, "data/");
			strcat(file_name, my_id_str);
			strcat(file_name, "_with_");
			strcat(file_name, chat_id_str);
	
			int fd;
			fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0664);
			if(fd == -1)
			{
				perror("open file");
				exit(0);
			}
			
			ticks = time(NULL); 
			sprintf(time_mes, "%s", ctime(&ticks));

			strcpy(save_message, time_mes);
			strcat(save_message, chat_id_str);
			strcat(save_message, " say: ");
			strcat(save_message, mes.message);
			strcat(save_message, "\n");

			writen(fd, save_message, 100);                               //保存发过来的私聊消息 
			
			if(strcmp(mes.message, "exit") == 0)                        //如果对方发送exit则退出 
			{
				printf("\n\n%s", time_mes);                             //显示时间和发送过来的消息 
				printf("\033[47;31mUser%d has leave private chat.\033[4A", chat_id);
				printf("\033[0m");
				printf("\n");
				
				goto pass1;                                             //退出时不显示对方发送过来的"exit"消息
			}
			
			printf("\t\t\t\t%s", time_mes);                             //显示时间和发送过来的消息
			printf("\t\t\t\t\033[47;30m%s\033[0m", mes.message);
			printf("\n\n");
			pass1:
				close(fd);   //attention!
		}
		
		else if(strcmp(mes.message, "group chat success") == 0)         //群聊连接成功
		{
			system("clear");
			printf("\nConnect success. Begin group chat...\n");
			printf("(Input \"exit\" if you want to quit.)\n");

			pthread_create(&tid, NULL, send_group_mes, &my_id);         //群聊发送消息
		}
		
		else if(strcmp(mes.com, "group_message") == 0)                  //接收群聊信息并显示
		{
			char my_id_str[10];
			char file_name[50];
			char id_who_sended[10];

			sprintf(my_id_str, "%d", my_id);
			sprintf(id_who_sended, "%d", mes.id_name);
	
			strcpy(file_name, "data/");
			strcat(file_name, my_id_str);
			strcat(file_name, "_group_chat_history");
	
			int fd;
			fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0664);
			if(fd == -1)
			{
				perror("open file");
				exit(0);
			}
			
			ticks = time(NULL); 
			sprintf(time_mes, "%s", ctime(&ticks));

			strcpy(save_message, time_mes);
			strcat(save_message, id_who_sended);
			strcat(save_message, " say: ");
			strcat(save_message, mes.message);
			strcat(save_message, "\n");

			writen(fd, save_message, 100);                               //保存群聊消息
			
			if(strcmp(mes.message, "exit") == 0)                        //如果对方发送exit则退出 
			{
				printf("%s", time_mes);     
				printf("\033[47;31mUser%d has leave group chat.\033[0m", mes.id_name);    
				printf("\n\n");
				
				goto pass;                                              //退出时不显示所发送的"exit"
			}
			
			if(mes.id_name == my_id)
			{
				goto pass;                                              //如果消息是自己发送的就不显示
			}
			
			printf("\t\t\t\t%s", time_mes);                             //显示群聊消息
			printf("\t\t\t\t\033[40;37mUser%d say: \033[0m", mes.id_name);   
			printf("\033[47;30m%s\033[0m\n\n",mes.message);
			
			pass:
				close(fd); //attention!
		}

		else if(strcmp(mes.com, "view_online_id") == 0)                 //接收并显示在线用户列表
		{
			system("clear");
			printf("%s", mes.message);
			
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单  
		}

		else if(strcmp(mes.com, "user_online") == 0)                    //显示其他用户上线消息
		{
			printf("\n\033[47;31m%s\033[0m\033[3A\n", mes.message);
			printf("\033[0m");
		}
		
		else
		{
			continue;
		}
	}
	
	exit(0);
}

/****************************************************
*	函数介绍：私聊线程函数  						*
*	实现功能：发送私聊信息	                		*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void *send_pri_mes(void *arg)
{
	int		 send_sockfd = *(int *)arg;
	char	 my_id_str[10];
	char	 chat_id_str[10];
	char	 file_name[20];
	MESSAGE  pri_data;
	
	sprintf(my_id_str, "%d", my_id);
	sprintf(chat_id_str, "%d", chat_id);
	
	strcpy(file_name, "data/");
	strcat(file_name, my_id_str);
	strcat(file_name, "_with_");
	strcat(file_name, chat_id_str);
	
	int fd;
	fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0664);
	if(fd == -1)
	{
		perror("open file");
		exit(0);
	}

	while(1)
	{
		printf("\n");
		strcpy(pri_data.com, "pri_data");
		pri_data.id = send_sockfd;
		
		mygets(pri_data.message, MAXLINE);
		ticks = time(NULL); 
		sprintf(time_mes, "%s", ctime(&ticks));
		printf("%s", time_mes);	

		strcpy(save_message, time_mes);
		strcat(save_message, "I say:");
		strcat(save_message, pri_data.message);

		writen(fd, save_message, 100);                                   //保存发过去的私聊消息
		
		if(strcmp(pri_data.message, "exit") == 0)
		{
			system("clear");
			printf("\nYou have quit the private chat.\n");
			writen(sockfd, &pri_data, sizeof(MESSAGE));
			
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单
			
			goto pass;
		}
	
		writen(sockfd, &pri_data, sizeof(MESSAGE));
	}
	
	pass:
		close(fd);   //attention

	return (NULL);
}

/****************************************************
*	函数介绍：群聊线程函数							*
*	实现功能：发送群聊信息   						*
*	输入参数：客户端id								*	
*	返回值  ：无									*
****************************************************/
void *send_group_mes(void *arg)
{
	int my_id = *(int *)arg;
	
	MESSAGE group_data;
	
	while(1)
	{
		printf("\n");
		strcpy(group_data.com, "group_message");
		
		mygets(group_data.message, MAXLINE);                            //输入私聊消息
		
		ticks = time(NULL); 
		sprintf(time_mes, "%s", ctime(&ticks));
		printf("%s", time_mes);	

		if(strcmp(group_data.message, "exit") == 0)
		{
			system("clear");
			printf("\nYou have quit the group chat.\n");
			writen(sockfd, &group_data, sizeof(MESSAGE));
			
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单  
			
			goto pass;	
		}
	
		writen(sockfd, &group_data, sizeof(MESSAGE));
	}
	
	pass:
		printf("Come here.^_^\n");
	
	return (NULL);
}

/****************************************************
*	函数介绍：第二级选择线程函数					*
*	实现功能：进行功能选择   						*
*	输入参数：客户端id  							*	
*	返回值  ：无									*
****************************************************/
void *next_select(void *arg)
{
	int local_my_id = *(int *)arg;
	
	char select = 0;
	show_menu_second();
	select = tolower(getchar());

	while((select != '\n') && (getchar() != '\n')); 
	switch(select)
	{
		case '1':
			view_online_id();
			break;
		case '2':
			private_chat();
			break;
		case '3':
			group_chat();
			break;
		case '4':
			view_chat_history();
			break;
		case '5':
			quit(local_my_id);
			system("clear");
			exit(0);
		default:
			system("clear");  
			printf("\n\tInput error！\n");
			
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单  
	}

	return (NULL);
}

/****************************************************
*	函数介绍：查看在线人数函数						*
*	实现功能：查看当前在数id						*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void view_online_id()
{
	MESSAGE view_online_id_data;
	
	strcpy(view_online_id_data.com, "view_online_id");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
}

/****************************************************
*	函数介绍：私聊选择函数							*
*	实现功能：向服务器发送私聊协议					*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void private_chat()
{
	system("clear");

	MESSAGE private_chat_data;
	strcpy(private_chat_data.com, "private_chat");

	printf("Please input the id you want to chat(1~9999):\n");
	scanf("%d", &private_chat_data.id);
	getchar();
	
	if(private_chat_data.id < 1 || private_chat_data.id > 9999)
	{
		system("clear");
		printf("\nInput error\n");
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
		
		goto pass;
	}

	chat_id = private_chat_data.id;                                     //保存私聊对象的id，用于创建消息记录文件名

	writen(sockfd, &private_chat_data, sizeof(MESSAGE));
	
	pass:
		;
}

/****************************************************
*	函数介绍：群聊选择函数							*
*	实现功能：发送群聊协议  						*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void group_chat()
{
	MESSAGE group_chat_data;
	
	strcpy(group_chat_data.com, "group_chat");

	writen(sockfd, &group_chat_data, sizeof(MESSAGE));
}

/****************************************************
*	函数介绍：查看历史记录函数						*
*	实现功能：进行私聊、群聊消息选择				*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void view_chat_history()
{
	int id_or_group_chat;
	
	system("clear");
	printf("\nPlease input the id you want to view,\nor input \"0\" to view the group chat history(1~9999):\n");
	scanf("%d", &id_or_group_chat);
	while(getchar() != '\n');
	
	if(id_or_group_chat < 0 || id_or_group_chat > 9999)
	{
		system("clear");
		printf("\nInput error.");
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
	}

	else if(id_or_group_chat == 0)
	{
		view_group_chat_history();
	}
	
	else if(id_or_group_chat < 9999 && id_or_group_chat != 0)
	{
		view_private_chat_history(id_or_group_chat);
	}
}

/****************************************************
*	函数介绍：查看私聊消息记录函数					*
*	实现功能：读取历史记录文件，并显示到终端		*
*	输入参数：查看的用户Id							*	
*	返回值  ：无									*
****************************************************/
void view_private_chat_history(int id_or_group_chat)
{
	char my_id_str[10];
	char chat_id_str[10];
	char find_file_name[20];

	sprintf(my_id_str, "%d", my_id);
	sprintf(chat_id_str, "%d", id_or_group_chat);

	strcpy(find_file_name, my_id_str);
	strcat(find_file_name, "_with_");
	strcat(find_file_name, chat_id_str);
	
	char *dir_name = "data";
	DIR *dir;
	dir = opendir(dir_name);
	if(dir == NULL)
	{
		perror("open data");
		exit(0);
	}

	struct dirent *tmp;
	int exist = 0;
	while((tmp = readdir(dir)) != NULL)
	{
		if(strcmp(tmp->d_name, find_file_name) == 0)
		{
			exist = 1;
			break;
		}
	}
	
	if(exist)
	{
		char find_file_name_with_dir[20];
		
		strcpy(find_file_name_with_dir, "data/");
		strcat(find_file_name_with_dir, find_file_name);
		
		int fd;
		fd = open(find_file_name_with_dir, O_RDONLY);
		if(fd == -1)
		{
			perror("open");
			exit(0);
		}
		
		system("clear");
		char read_message[100];
		while(read(fd, read_message, 100) > 0)
		{
			printf("%s\n", read_message);
		}
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);               //返回到第二层菜单  
		
		goto pass;
	}

	else
	{
		system("clear");
		printf("\nThere is not the private chat history with user %d.\n", id_or_group_chat);
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
	
		goto pass;
	}

	pass:
		printf("Come here. ^_^\n");
}

/****************************************************
*	函数介绍：查看群聊历史记录函数					*
*	实现功能：读取群聊历史记录，并显示到终端		*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void view_group_chat_history()
{
	char my_id_str[10];
	char find_file_name[50];

	sprintf(my_id_str, "%d", my_id);
	
	strcpy(find_file_name, my_id_str);
	strcat(find_file_name, "_group_chat_history");
	
	char find_file_name_with_dir[50];                                   
	strcpy(find_file_name_with_dir, "data/");
	strcat(find_file_name_with_dir, find_file_name);
	
	int fd;
	fd = open(find_file_name_with_dir, O_RDONLY);
	if(fd != -1)                                 //可以不要像私聊那样打开目录再寻找文件是否存在，这里直接打开文件，判断文件是否存在
	{
		system("clear");
		
		char read_message[100];
		while(read(fd, read_message, 100) > 0)
		{
			printf("%s\n", read_message);
		}
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
		
		goto pass;
	}

	else
	{
		system("clear");
		printf("\nThere is not the group chat history.\n");
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);               
		
		goto pass;
	}
	
	pass:
		printf("Come here.^_^\n");
}

/****************************************************
*	函数介绍：退出客户端函数						*
*	实现功能：向服务器发送退出协议					*
*	输入参数：客户端Id								*	
*	返回值  ：无									*
****************************************************/
void quit(int my_id)
{
	MESSAGE quit_data;
	
	strcpy(quit_data.com, "exit");
	quit_data.id = my_id;

	writen(sockfd, &quit_data, sizeof(MESSAGE));
}

/****************************************************
*	函数介绍：登录界面函数							*
*	实现功能：进行登录、注册选择					*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void *sel(void *arg)
{
	char select = 0;
		
	show_menu_first();
	
	select = tolower(getchar());
	while((select != '\n') && (getchar() != '\n'));  
	
	switch(select)
	{
		case '1':
			login();
			break;
		case '2':
			regist();
			break;
		case '3':
			system("clear");
			exit(0);
		default:
			system("clear");  
			printf("\n\tInput error!\n");
			
			press_any_one();	
			pthread_create(&tid, NULL, sel, NULL);
	}
	return(NULL);
}

/****************************************************
*	函数介绍：登录函数								*
*	实现功能：输入登录id和密码						*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void login()
{
	system("clear");

	MESSAGE login_data;
	strcpy(login_data.com, "login");

	printf("Please input your id(1~9999):");
	scanf("%d", &login_data.id);
	while(getchar() != '\n');
	
	if(login_data.id < 1 || login_data.id > 9999)
	{
		system("clear");
		printf("\nInput error.\n");
		exit(0);
	}

	strcpy(login_data.message, getpass("Please input your passwd:"));

	writen(sockfd, &login_data, sizeof(MESSAGE));
}

/****************************************************
*	函数介绍：注册函数								*
*	实现功能：输入注册Id和密码						*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void regist()
{
	system("clear");

	MESSAGE regist_data;
	strcpy(regist_data.com, "regist");

	printf("\nPlease input the id you want to regist(1~9999):");
	scanf("%d", &regist_data.id);
	while(getchar() != '\n');

	if(regist_data.id < 1 || regist_data.id > 9999)
	{
		system("clear");
		printf("\nInput error. Regist failed.\n");
		exit(0);
	}
	
	char input_passwd_first[MAXLINE];
	char input_passwd_second[MAXLINE];

	strcpy(input_passwd_first, getpass("Please input your passwd:"));
	strcpy(input_passwd_second, getpass("Please input your passwd again:"));

	if(strcmp(input_passwd_first, input_passwd_second) != 0)
	{
		system("clear");
		printf("\nThe password you input twice is not same. Regist failed.\n");
		exit(0);
	}
	
	else
	{
		strcpy(regist_data.message, input_passwd_second);
	}

	writen(sockfd, &regist_data, sizeof(MESSAGE));
}

/****************************************************
*	函数介绍：输入字符串函数						*
*	实现功能：替代系统函数gets						*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void mygets(char buf[], int len)
{
	int i = 0;
	
	for(i = 0; i < len-1; i++)  
	{
		buf[i] = getchar();
		
		if(buf[i] == '\n')
		{
			break;
		}
	}
	
	if(buf[i] != '\n')
	{
		while(getchar() != '\n');
	}
	
	buf[i] = '\0';
}

/****************************************************
*	函数介绍：暂停函数								*
*	实现功能：按enter键继续 						*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void press_any_one()
{
	printf("\nPress Enter to continue\n");
	getchar();
}

/****************************************************
*	函数介绍：第一级界面函数						*
*	实现功能：显示界面      						*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void show_menu_first()
{
	system("clear");
	printf(
			"\n\t\033[42;30mWelcome\033[1m\033[0m"
			"\n\n\t1)---Login"
			"\n\n\t2)---Regist"
			"\n\n\t3)---Quit"
			"\n\n\tInput your choose:\n"
	  		);
}

/****************************************************
*	函数介绍：第二层界面函数						*
*	实现功能：进行功能选择  						*
*	输入参数：无									*	
*	返回值  ：无									*
****************************************************/
void show_menu_second()
{
	system("clear");
	printf(
			"\n"
			"\n\n\t1)---View the online user"
			"\n\n\t2)---Private chat"
			"\n\n\t3)---Group chat"
			"\n\n\t4)---View chat history"
			"\n\n\t5)---Quit"
			"\n\n\tInput your choose:\n"
	  		);
}

/****************************************************
*	函数介绍：读取服务器消息函数    				*
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
*	函数介绍：发送消息给服务器函数    				*
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

