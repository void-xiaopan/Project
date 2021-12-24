#include <stdio.h>
#include <time.h>
#include <sqlite3.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

/*variate*/
typedef void (*sighandler_t)(int);

typedef struct msg_buf
{
	char type;
	char err;
	char act;
	char name[20];
	char passwd[20];
	char data[512];	
}__attribute__((packed))MSG, *PMSG;
typedef struct arg_buf
{
	int sfd;
	struct sockaddr_in addr;
	PMSG pbuf;
}ARG, *PARG;


/*macro*/
#define regist	'r'
#define login	'l'
#define word_ex	'w'
#define history	'h'
#define quit	'q'
#define cancel 	'c'
#define ERR(A, B) 	do{\
	if(A)\
	{\
		perror(B);\
		return -1;\
	}\
	else\
	{\
		puts(B);\
	}\
}while(0)

#define PORT 8888
#define ADDR "192.168.1.120"

/*function*/
int GetHistory(sqlite3 *db, PARG All_buf);	
int GetWordResults(sqlite3 *db, PARG All_buf);
void signal_handler(int signum);
int FindWord(sqlite3 *db, PARG All_buf);
int do_regist(PARG All_buf);
int FindData(sqlite3* db, PARG All_buf);
int child_sesion(PARG All_buf);
int Login_Verify(sqlite3 *db, PARG All_buf);
int do_login(PARG All_buf);
int verify(sqlite3 *db, PARG All_buf);
int sql_exec(sqlite3* db, char *sql);
int GetTime(char *Time);
int Edict(sqlite3 *db, PARG All_buf);
int create_dict(sqlite3 *db);
int UsrCancel(sqlite3 *db, PARG All_buf);



int main(int argc, char **argv)
{

	int sfd = socket(AF_INET, SOCK_STREAM, 0);	
	ERR(sfd == -1, "socket");
	int resuse = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &resuse, sizeof(resuse));
	sighandler_t sig_ret;
	sig_ret = signal(SIGCHLD, signal_handler);
	ERR(sig_ret == SIG_ERR, "signal");
	struct sockaddr_in ser_addr, cli_addr;
	bzero(&ser_addr, sizeof(ser_addr));
	bzero(&cli_addr, sizeof(cli_addr));
	int cli_len = sizeof(cli_addr);
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(PORT);
	ser_addr.sin_addr.s_addr = inet_addr(ADDR);
	int bin_ret = bind(sfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	ERR(bin_ret == -1, "bind");
	int lis_ret = listen(sfd, 10);
	ERR(lis_ret == -1, "listen");
	pid_t pid;
	int ac_fd;
	ARG All_buf;	//定义一个总的结构体
	bzero(&All_buf, sizeof(All_buf));
	MSG MsgBuf;
	bzero(&MsgBuf, sizeof(MsgBuf));
	while(1)
	{
		All_buf.pbuf = &MsgBuf;
		ac_fd = accept(sfd, (struct sockaddr *)&cli_addr, &cli_len);
		ERR(ac_fd == -1, "accept");
		All_buf.sfd = ac_fd;
		All_buf.addr = cli_addr;
		pid = fork();
		if(pid == 0)
		{
			if(child_sesion(&All_buf) == -1)		//对接客户端连接
			{
				printf("child_sesion\n");
			}
			else
				printf("child!\n");
			_Exit(0);
		}else if(pid > 0)
		{

			bzero(&All_buf, sizeof(All_buf));
			printf("parence\n");
			sleep(1);

		}else
		{
			perror("fork");
		}
	}
	close(sfd);
	return 0;
}

int child_sesion(PARG All_buf)
{
	MSG recv_buf;
	while(1)
	{
		bzero(&recv_buf, sizeof(recv_buf));
		bzero(All_buf->pbuf, sizeof(MSG));		//把传参的机构体清零
		All_buf->pbuf->type = 0;
		ssize_t r_ret = recv(All_buf->sfd, &recv_buf, sizeof(recv_buf), 0);
		ERR(r_ret == -1, "recv");
		if(r_ret == 0)
		{
			break;
		}
		All_buf->pbuf->type = recv_buf.type;
	
		strcpy(All_buf->pbuf->name, recv_buf.name);
		strcpy(All_buf->pbuf->passwd, recv_buf.passwd);
		switch(recv_buf.type)
		{
			case regist:
				if(do_regist(All_buf) == -1)
					printf("errro: __%d__\n", __LINE__);
				else
				{
					printf("regist\n");	//
				}
				break;
			case login:
				if(do_login(All_buf) != 0)
				{
					printf("error: __%d__\n", __LINE__);
				}
				else
				{
					printf("login\n");	//
				}
				break;
			case quit:
				printf("quit!\n");
				return 1;
				break;
			default:
				printf("error!\n");
				break;		 

		}
	}
	return 0;

}

int do_login(PARG All_buf)
{
	int var = 0;
	sqlite3 *db = NULL;
	int o_ret = sqlite3_open("./data.db", &db);
	if(o_ret != SQLITE_OK)
	{
		printf("error: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	var = Login_Verify(db, All_buf);
	switch(var)
	{
		case 0:
			Edict(db, All_buf);
			break;
		case 1:
		case -1:
			break;
		default:
			break;
	}	
	
	while(sqlite3_close(db) != SQLITE_OK);
	return var;
}

int Edict(sqlite3 *db, PARG All_buf)
{
	int var = 0;
	while(1)
	{
		recv(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
		switch(All_buf->pbuf->act)
		{
			case word_ex:
				FindWord(db, All_buf);
				printf("word!\n");
				break;
			case history:
				GetHistory(db, All_buf);
				printf("history\n");
				break;
			case cancel:
				var = 1;
				UsrCancel(db, All_buf);
				printf("cancel\n");
				break;	
		}
		if(var == 1)
		{
			break;
		}
	
	}
	return 0;
}

int UsrCancel(sqlite3 *db, PARG All_buf)
{
	bzero(All_buf->pbuf->data, 512);
	strcpy(All_buf->pbuf->data, "SEND_OVER");
	send(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
	return 0;
}

int GetHistory(sqlite3 *db, PARG All_buf)
{
	int flags = 0;
	char data[512] = {0};
	char sql[128] = "select * from log;";
	char** pres = NULL;
	int row, column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db, sql, &pres, &row, &column, &errmsg) != 0)
	{
		printf("sqlite3_get_table: %s   __%d__\n", errmsg, __LINE__);
		return -1;
	}
	int i, j;
	if(row > 0)
	{
		bzero(All_buf->pbuf->data, 512);
		sprintf(All_buf->pbuf->data, "%s %s %s", pres[0], pres[1], pres[2]);
		send(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
		if(row < 10)
		{
			for(i = 1; i <= row; i++)
			{
				bzero(All_buf->pbuf->data, 512);
				sprintf(All_buf->pbuf->data, "%s %s %s", pres[i*column], pres[i*column+1], pres[i*column+2]);
				send(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
			}
		}
		else if(row > 10)
		{
			for(i = row-10; i <= row; i++)
			{
				bzero(All_buf->pbuf->data, 512);
				sprintf(All_buf->pbuf->data, "%s %s %s", pres[i*column], pres[i*column+1], pres[i*column+2]);
				send(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
			}

		}
	}
	else 
	{
		bzero(All_buf->pbuf->data, 512);
		sprintf(All_buf->pbuf->data, "%s", "近期没有客户使用记录");
		send(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
	}
	bzero(All_buf->pbuf->data, 512);
	strcpy(All_buf->pbuf->data, "SEND_OVER");
	send(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
	sqlite3_free_table(pres);

	return flags;
	
	
}


int create_dict(sqlite3 *db)
{

	char sql[512] = "create table if not exists dict (word char, mean char);";
	char* errmsg = NULL;
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("ERROR:%s __%d__\n", errmsg, __LINE__);
		return -1;
	}
//	printf("create table success\n");
	
	//打开dict.txt
	FILE* fp = fopen("./dict.txt", "r");
	if(NULL == fp)
	{
		perror("fopen");
		return -1;
	}

	char buf[BUFSIZ] = "";
	char word[100] = "";
	char mean[200] = "";
	//循环读取一行，将word和mean提取出来
	while(fgets(buf, sizeof(buf), fp) != NULL)
	{
		//%[^\n]:提取数据直到'\n'字符为止，不包含'\n'字符
		sscanf(buf, "%s %[^\n]", word, mean);
		//		printf("%s %s\n", word, mean);

		//写入到数据库中
		bzero(sql, sizeof(sql));
		sprintf(sql, "insert into dict values(\"%s\",\"%s\")", word, mean);
		if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
		{
			printf("ERROR:%s __%d__\n", errmsg, __LINE__);
			return -1;
		}

		bzero(word, sizeof(word));
		bzero(mean, sizeof(mean));
	}


	//关闭dict,txt
	fclose(fp);
	return 0;
}

int GetTime(char *tm_buf)
{
	int rand(void);
	time_t tm1 = time(NULL);
	struct tm *nowtm = localtime(&tm1);
	sprintf(tm_buf, "%2d-%2d %2d:%2d", nowtm->tm_mon+1, nowtm->tm_mday, nowtm->tm_hour, nowtm->tm_min);
	return 0;
}

int FindWord(sqlite3 *db, PARG All_buf)
{
	create_dict(db);
	char sql[768] = {0};
	char tm_buf[128] = {0};
	GetTime(tm_buf);
	sprintf(sql, "create table if not exists log(time char, word char, exist char);");
	sql_exec(db, sql);
	bzero(sql, sizeof(sql));
	int ret;
	ret = GetWordResults(db, All_buf);		//获取单词查询结果0表示不存在 1表示存在
	if(ret == 0)
	{
		sprintf(sql, "insert into log values(\"%s\",\"%s\", \"%s\");", tm_buf, All_buf->pbuf->data, "NO");
		sql_exec(db, sql);

	}
	else if(ret == 1)
	{
		sprintf(sql, "insert into log values(\"%s\",\"%s\",\"%s\");", tm_buf, All_buf->pbuf->data, "YES");
		sql_exec(db, sql);
	}
	bzero(All_buf->pbuf->data, 512);
	strcpy(All_buf->pbuf->data, "SEND_OVER");
	send(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
	return 0;
}

int GetWordResults(sqlite3 *db, PARG All_buf)
{
	int flags = 0;
	char word[512] = {0};
	strcpy(word, All_buf->pbuf->data);
	char sql[128] = "select * from dict;";
	char** pres = NULL;
	int row, column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db, sql, &pres, &row, &column, &errmsg) != 0)
	{
		printf("sqlite3_get_table: %s   __%d__\n", errmsg, __LINE__);
		return -1;
	}
	int i = 0;
	for(i = 1; i <= row; i++)
	{
		if(strcmp(word, pres[i*column]) == 0)
		{
			flags = 1;
			bzero(All_buf->pbuf->data, 512);
			sprintf(All_buf->pbuf->data, "%20s   %s", pres[i*column], pres[i*column+1]);
			send(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
			break;	
		}
	}
	if(flags == 0)
	{
		bzero(All_buf->pbuf->data, 512);
		sprintf(All_buf->pbuf->data, "%20s %s", word, "不存在，请重新输入");
		send(All_buf->sfd, All_buf->pbuf, sizeof(MSG), 0);
	}
	sqlite3_free_table(pres);

	return flags;
}


int Login_Verify(sqlite3 *db, PARG All_buf)
{
	int var = 0;
	char name[20] = {0};
	int sfd = All_buf->sfd;
	char passwd[20]	= {0};
	strcpy(name, All_buf->pbuf->name);
	strcpy(passwd, All_buf->pbuf->passwd);
	int flags = 2;
	flags = FindData(db, All_buf);	//查找数据判定是否重复
	if(flags == 0)	//bucunzai 
	{
		All_buf->pbuf->err = 1;
		strcpy(All_buf->pbuf->data, "账号不存在，请先注册");
		send(sfd, All_buf->pbuf, sizeof(MSG), 0);
		var = 1;
	}
	else if(flags == 1 )	//cunzai 
	{
		printf("账号存在\n");
		int ver_ret = 3;
		while(verify(db, All_buf) != 2)
		{
			All_buf->pbuf->err = 3;
			strcpy(All_buf->pbuf->data, "密码输入错误，请重新检查密码");
			send(sfd, All_buf->pbuf, sizeof(MSG), 0);
			recv(sfd, All_buf->pbuf, sizeof(MSG), 0);
		}
		All_buf->pbuf->err = 0;
		bzero(All_buf->pbuf->data, 512);
		strcpy(All_buf->pbuf->data, "账户登录成功");
		send(sfd, All_buf->pbuf, sizeof(MSG), 0);
		
	}
	else 
	{
		printf("verify or system is error!\n");
		var = -1;
	}
	return var;
}

int verify(sqlite3 *db, PARG All_buf)
{
	int flags = 0;
	char name[20] = {0};
	strcpy(name, All_buf->pbuf->name);
	char passwd[20] = {0};
	strcpy(passwd, All_buf->pbuf->passwd);
	char sql[128] = "select * from usrinfor;";
	char** pres = NULL;
	int row, column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db, sql, &pres, &row, &column, &errmsg) != 0)
	{
		printf("sqlite3_get_table: %s   __%d__\n", errmsg, __LINE__);
		return -1;
	}
	int i = 0;
	for(i = 1; i <= row; i++)
	{
		if(strcmp(name, pres[i*column]) == 0)
		{
			flags = 1;
			if(strcmp(passwd, pres[i*column+1]) == 0)
			{
				flags = 2;
			}
			break;	
		}
	}
	
	sqlite3_free_table(pres);
	return flags;
}

int do_regist(PARG All_buf)		//账号注册操作
{
	int sfd = All_buf->sfd;
	char name[20] = {0};
	strcpy(name, All_buf->pbuf->name);
	char passwd[20] = {0};
	strcpy(passwd, All_buf->pbuf->passwd);
	sqlite3 *db = NULL;
	int o_ret = sqlite3_open("./data.db", &db);
	if(o_ret != SQLITE_OK)
	{
		printf("error: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	printf("opened!\n");//打开sql成功标志
	char sql[128] = {0};
	strcpy(sql, "create table if not exists usrinfor(name char primary key, passwd char);");
	MSG send_buf;		
	bzero(&send_buf, sizeof(send_buf));
	sql_exec(db, sql);
	bzero(sql, sizeof(sql));
	int flags = 2;
	flags = FindData(db, All_buf);	//查找数据判定是否重复
	if(flags == 0)
	{

		sprintf(sql, "insert into usrinfor values (\"%s\",\"%s\" );", name, passwd);
		sql_exec(db, sql);
		strcpy(send_buf.data, "账号注册成功");
		send(sfd, &send_buf, sizeof(send_buf), 0);
	}
	else if(flags == 1)
	{
		strcpy(send_buf.data, "账号已存在，请登录");
		send(sfd, &send_buf, sizeof(send_buf), 0);
	}
	else
	{
		printf("FindData is error!\n");
	}
	if(sqlite3_close(db) != SQLITE_OK)
	{
		printf("error: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	return 0;
}
int FindData(sqlite3* db, PARG All_buf)		//验证账号信息是否存在
{
	int flags = 0;
	char name[20] = {0};
	strcpy(name, All_buf->pbuf->name);
	char passwd[20] = {0};
	strcpy(passwd, All_buf->pbuf->passwd);
	char sql[128] = "select * from usrinfor;";
	char** pres = NULL;
	int row, column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db, sql, &pres, &row, &column, &errmsg) != 0)
	{
		printf("sqlite3_get_table: %s   __%d__\n", errmsg, __LINE__);
		return -1;
	}
	int i = 0;
	for(i = 1; i <= row; i++)
	{
		if(strcmp(name, pres[i*column]) == 0)
		{
			flags = 1;
			break;	
		}
	}
	sqlite3_free_table(pres);
	return flags;
}

int sql_exec(sqlite3* db, char *sql)
{
	char *errmsg = NULL;
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("error: %s\n", errmsg);
		return -1;
	}
	printf("sql_execed!\n");
	return 0;
}


void signal_handler(int signum)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
	printf("The child quit!\n");

}
