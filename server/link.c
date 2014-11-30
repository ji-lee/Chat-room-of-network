#include"link.h"


/****************************************************
*	实现功能：添加用户节点							*
*	输入参数：用户链表和用户结构体					*
*	返回值  ：无                         			*
****************************************************/
void add_node(PNODE head, PNODE_DATA pnode_data)
{
	PNODE temp = create_node(pnode_data);
	temp->next = head->next;
	head->next = temp;
	printf("\n新用户添加完成.\n");
}

/****************************************************
*	实现功能：查找用户节点							*
*	输入参数：用户链表和用户id						*
*	返回值  ：找到节点的前一个节点或NULL		    *
****************************************************/
PNODE find_pre_node(PNODE head, int id)
{
	if(NULL == head->next)
	{
		return NULL;
	}
	PNODE q = head;
	PNODE p = head->next;
	while(p != NULL)
	{
		if(p->pnode_data.id == id)
		{
			return q;
		}
		q = q->next;
		p = p->next;
	}
	return NULL;
}

/****************************************************
*	实现功能：创建用户节点							*
*	输入参数：用户结构体数据					   	*
*	返回值  ：新创建的用户节点						*
****************************************************/
PNODE create_node(PNODE_DATA pnode_data)
{
	PNODE temp = (PNODE)malloc(sizeof(NODE));
	temp->pnode_data = pnode_data;
	temp->next = NULL;
//	printf("创建完成．\n");
	return temp;
}

/****************************************************
*	实现功能：从文件中加载用户数据					*
*	输入参数：无　　						    	*
*	返回值  ：用户链表								*
****************************************************/
PNODE load_data()
{
	FILE *fp;
	if(NULL == (fp = fopen("data", "r")))
	{
		if(NULL == (fp = fopen("data", "w+")))
		{
			system("clear");
			printf("\n硬盘文件内容加载失败．请重新加载．\n");
			exit(0);
		}	
	}
	
	PNODE head = (PNODE)malloc(sizeof(NODE));
	head->next = NULL;
	PNODE_DATA pnode_data; 
	
	while(fread(&pnode_data, sizeof(PNODE_DATA), 1, fp) != 0)
	{
		pnode_data.sockfd = -1;
		
		add_node(head, pnode_data);
	}
	
//	system("clear");
	printf("\n数据加载成功．\n");

	fclose(fp);
	fp = NULL;
	
	return head;
}

/****************************************************
*	实现功能：保存用户数据			　　	    	*
*	输入参数：用户链表								*
*	返回值  ：无							    	*
****************************************************/
void save_data(PNODE head)
{
	FILE *fp;
	if(NULL == (fp = fopen("data", "w+")))
	{
		system("clear");
		printf("\n数据保存失败，请重新保存．\n");
		exit(0);
	}
	if(NULL == head->next)
	{
		printf("退出时链表为空，不需要保存数据．\n");
		return;
	}
	
	PNODE p = head->next;
	while(p != NULL)
	{
		fwrite(&p->pnode_data, sizeof(PNODE_DATA), 1, fp);
		p = p->next;
	}

//	printf("\n用户数据保存成功．\n");
	fclose(fp);
	fp = NULL;
}

