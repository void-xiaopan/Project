#include "staff_management_include.h"

int main(int argc, char **argv)
{
	int sfd = 0, ret;
	int newfd = 0;
	pid_t pid;
	sighandler_t sig = signal(SIGCHLD, signal_handler);
	if(sig == SIG_ERR)
	{
		printf("signal is error!\n");
		return -1;
	}
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1)
	{
		perror("socket:");
		return -1;
	}
	int reuse = 1;
	if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		printf("sercockopt is error!\n");
		ret = -1;
		goto ERR;
	}
			
	struct sockaddr_in *client_addr = (struct sockaddr_in *)__malloc_function_(sizeof(struct sockaddr_in));
	int addr_len = sizeof(struct sockaddr_in);
	struct sockaddr_in *server_addr = (struct sockaddr_in *)__malloc_function_(sizeof(struct sockaddr_in));
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(PORT);
	server_addr->sin_addr.s_addr = inet_addr(IP);
	ret = bind(sfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr_in));
	ret = listen(sfd, 10);
	if(ret < 0)
	{
		printf("listen is error!\n");	
		goto ERR1;
	}
	while(1)
	{
		printf("等待连接中.....\n");
		newfd = accept(sfd, (struct sockaddr *)client_addr, (socklen_t *)&addr_len);
		if(newfd == -1)
		{
			perror("accept:");
			ret = newfd;
			goto ERR1;
		}
		pid = fork();
		if(pid == 0)
		{
			printf("%d 连接成功!\n", newfd);
			PMSG pmsg = (PMSG)__malloc_function_(sizeof(MSG));
			while(1)
			{
				if(The_server_parses_client_operations(pmsg, &newfd) == 3)//服务器解析客户端操作 
				{
					Records_the_login_status_bits(pmsg, &newfd, 0);
					general_staff_Records_the_login_status_bits(pmsg, &newfd, 0);//普通员工记录登录状态位_
					break;
				}
			}
			printf("%d\n", newfd);
			__free_function((void **)&pmsg);
			break;
		}
		else if(pid > 0)
		{

		}
		else
		{
			printf("fork is error!\n");
		}
	}
	



	__free_function((void **)&server_addr);
	__free_function((void **)&client_addr);
	close(newfd);
	close(sfd);
	return 0;
/*========================error=========================*/
ERR1:
	__free_function((void **)&server_addr);
	__free_function((void **)&client_addr);
	close(newfd);
	close(sfd);
ERR:
	return ret;
}
