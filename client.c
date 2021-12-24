#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

/*variate*/

typedef struct msg_buf
{
	char type;
	char err;
	char act;
	char name[20];
	char passwd[20];
	char data[512];	
}__attribute__((packed))MSG, *PMSG;
typedef struct arg_buf		//
{
	int sfd;
	struct sockaddr_in addr;
	PMSG pbuf;
}ARG, *PARG;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;



/*macro*/
#define ERR(A, B) 	do{\
	if(A)\
	{\
		perror(B);\
		_Exit(-1);\
	}\
	else\
	{\
		puts(B);\
	}\
}while(0)

#define PORT 8888
#define ADDR "192.168.1.120"
#define regist	'r'
#define login	'l'
#define word	'w'
#define history	'h'
#define quit 	'q'
#define cancel 	'c'

/*function*/
int vip_act(int sfd, PMSG pbuf);
void PrintAppPic();
int do_regist(int sfd, PMSG pbuf);
int do_login(int sfd, PMSG pbuf);
int GetHistory(int sfd, PMSG pbuf);
int FindWord(int sfd, PMSG pbuf);
void PrintMSG();
void *handler (void *arg);
int Canel(int sfd, PMSG pbuf);



int main(int argc, char **argv)
{
 	int sfd = socket(AF_INET, SOCK_STREAM, 0);	
	ERR(sfd == -1, "socket");
	int resuse = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &resuse, sizeof(resuse));
	struct sockaddr_in ser_addr, cli_addr;
	bzero(&ser_addr, sizeof(ser_addr));
	bzero(&cli_addr, sizeof(cli_addr));
	int cli_len = sizeof(cli_addr);
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(PORT);
	ser_addr.sin_addr.s_addr = inet_addr(ADDR);
	int con_ret = connect(sfd, (struct sockaddr *)&ser_addr,sizeof(ser_addr));
	ERR(con_ret == -1, "connect");
	pthread_t pth;
	MSG send_buf;
	bzero(&send_buf, sizeof(send_buf));
	ARG A_buf;
	bzero(&A_buf, sizeof(ARG));
	A_buf.sfd = sfd;
	A_buf.pbuf = &send_buf;
	int p_ret = pthread_create(&pth, NULL, handler, &A_buf);
	ERR(p_ret != 0, "pthread_create");
	int ret_var = 0;
	while(1)
	{
		PrintMSG();
		scanf("%c", &send_buf.type);
		while(getchar() != '\n');
		switch(send_buf.type)
			{
				case regist:
					if(do_regist(sfd, &send_buf) == -1)
						printf("errro: __%d__\n", __LINE__);
					else
						printf("regist\n");	//
					break;
				case login:
					do_login(sfd, &send_buf);
					printf("logined\n");	//
					break;
				case quit:
					printf("quit\n");
					ret_var = 1;
					break;
				default:
					printf("error!\n");
					break;		 

			}
		bzero(&send_buf, sizeof(send_buf));
		if(ret_var == 1)
		{
			break;
		}
	}
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	close(sfd);
	return 0;
}
int do_login(int sfd, PMSG pbuf)
{
	MSG recv_buf;
	bzero(&recv_buf, sizeof(recv_buf));
	printf("请输入你要登录的账户名:\n");
	fgets(pbuf->name, 20, stdin);
	pbuf->name[strlen(pbuf->name)-1] = 0;
	printf("请输入你要登录账户的密码:\n");
	fgets(pbuf->passwd, 20, stdin);
	pbuf->passwd[strlen(pbuf->passwd)-1] = 0;
	ssize_t s_ret = send(sfd, pbuf, sizeof(MSG), 0);	//1
	ERR(s_ret == -1, "send");
	while(1)
	{
		ssize_t r_ret = recv(sfd, &recv_buf, sizeof(MSG), 0);
		ERR(r_ret == -1, "recv");
//		printf("recv_buf.err = %d\n", recv_buf.err);
		if(recv_buf.err == 0)
		{
			printf("%s\n", recv_buf.data);
			vip_act(sfd, pbuf);	//vip操作
			break;
		}
		else if(recv_buf.err == 3)
		{
			printf("%s\n", recv_buf.data);
			bzero(&recv_buf, sizeof(recv_buf));
			bzero(pbuf->passwd, 20);
			fgets(pbuf->passwd, 20, stdin);
			pbuf->passwd[strlen(pbuf->passwd)-1] = 0;
			ssize_t s_ret = send(sfd, pbuf, sizeof(MSG), 0);
			ERR(s_ret == -1, "send");

		}
		else if(recv_buf.err == 1)
		{
			printf("%s\n", recv_buf.data);
			break;
		}
	}
	return 0;
}


int vip_act(int sfd, PMSG pbuf)
{
	int s_ret;
	int flags = 0;
	char act = 0;
	while(1)
	{
		PrintAppPic();
		scanf("%c", &act);
		while(getchar() != '\n');
		bzero(pbuf->data, 512);	
		pbuf->act = act;
		switch(act)
		{
			case word:
				pthread_mutex_lock(&mutex);
				FindWord(sfd, pbuf);
				break;
			case history:
				pthread_mutex_lock(&mutex);
				GetHistory(sfd, pbuf);
				break;
			case cancel:
				Canel(sfd, pbuf);
				flags = 1;
				break;
		}
		printf("=============================================================\n");
		if(flags == 1)
		{
			break;
		}
		pthread_cond_wait(&cond, &mutex);
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

int Canel(int sfd, PMSG pbuf)
{
	send(sfd, pbuf, sizeof(MSG), 0);
	return 0;
}

int GetHistory(int sfd, PMSG pbuf)
{
	send(sfd, pbuf, sizeof(MSG), 0);
	return 0;
}

int FindWord(int sfd, PMSG pbuf)
{
	printf("请输入你要查找的单词:\n");
	bzero(pbuf->data, 512);
	fgets(pbuf->data, 512, stdin);
	pbuf->data[strlen(pbuf->data)-1] = 0;
	send(sfd, pbuf, sizeof(MSG), 0);
	return 0;
}

int do_regist(int sfd, PMSG pbuf)
{
	MSG recv_buf;
	bzero(&recv_buf, sizeof(recv_buf));
	printf("请输入你要注册的账户名:\n");
	fgets(pbuf->name, 20, stdin);
	pbuf->name[strlen(pbuf->name)-1] = 0;
	printf("请输入你要注册的账户的密码:\n");
	fgets(pbuf->passwd, 20, stdin);
	pbuf->passwd[strlen(pbuf->passwd)-1] = 0;
	ssize_t s_ret = send(sfd, pbuf, sizeof(MSG), 0);
	ERR(s_ret == -1, "send");
	ssize_t r_ret = recv(sfd, &recv_buf, sizeof(MSG), 0);
	ERR(r_ret == -1, "recv");
	printf("%s\n", recv_buf.data);
	return 0;
}

void PrintAppPic()
{
	printf("=============================================================\n");
	printf("=======================请选择你的操作========================\n");
	printf("=======================1.查找单词（w）=======================\n");
	printf("=======================2.查询历史记录（h）===================\n");
	printf("=======================3.退出登录（c）=======================\n");
	printf("=============================================================\n");
}

void PrintMSG()
{
	printf("=============================================================\n");
	printf("===============请输入对应的字符选择你需要的操作==============\n");
	printf("=======================1.注册账号（r）=======================\n");
	printf("=======================2.登录账号（l）=======================\n");
	printf("=======================3.退出客户端（q）=====================\n");
	printf("=============================================================\n");
}

void *handler (void *arg)
{
	PARG A_buf = (PARG)arg;
	int sfd = A_buf->sfd;
	pthread_detach(pthread_self());
	MSG recv_buf;
	ssize_t r_ret;
	while(1)
	{
		recv_buf.type = A_buf->pbuf->type;
		recv_buf.err = A_buf->pbuf->err;
		recv_buf.act = A_buf->pbuf->act;
		strcpy(recv_buf.name, A_buf->pbuf->name);
		strcpy(recv_buf.passwd, A_buf->pbuf->passwd);
		strcpy(recv_buf.data, A_buf->pbuf->data);
		if(recv_buf.type == login && (recv_buf.act == word || recv_buf.act == cancel || recv_buf.act == history))
		{
			bzero(&recv_buf, sizeof(recv_buf));
			r_ret = recv(sfd, &recv_buf, sizeof(MSG), 0);
			if(r_ret == 0)
			{
				printf("服务端异常\n");
				break;
			}
			else if(r_ret < 0)
			{
				perror("recv");
				break;
			}
			if(strcmp(recv_buf.data, "SEND_OVER") == 0)
			{
				pthread_mutex_lock(&mutex);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex);
			}
			else
			{
				printf("%s\n", recv_buf.data);
			}
			bzero(&recv_buf, sizeof(recv_buf));
		}
		else 
		{
			usleep(10000);
		}
	}
	pthread_exit(NULL);
}
