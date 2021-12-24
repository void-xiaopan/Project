#include "staff_management_include.h"
int *glo_sfd = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


int main(int argc, char **argv)
{
	int ret, sfd = 0;
	pthread_t th1;
	struct sockaddr_in *server_addr = (struct sockaddr_in *)__malloc_function_(sizeof(struct sockaddr_in));
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(PORT);
	server_addr->sin_addr.s_addr = inet_addr(IP);
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1)
	{
		perror("socket:");
		ret = sfd;
		goto ERR1;
	}
	

	ret = connect(sfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr_in));
	if(ret != 0)
	{
		perror("connect:");
		goto ERR1;
	}
	PMSG pmsg = (PMSG)__malloc_function_(sizeof(MSG));
	glo_sfd = &sfd;
	ret = pthread_create(&th1, NULL, pthread_func, (void *)pmsg);
	if(ret != 0)
	{
		printf("pthread create is error!\n");
		return -1;
	}
	while(1)
	{
		ret = user_connection_operation(pmsg);//用户连接操作 
		if(ret == 3)
		{
			pthread_cancel(th1);
			break;
		}
	}

	pthread_join(th1, NULL);
	close(sfd);
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
	__free_function_((void **)&pmsg);
	__free_function_((void **)&server_addr);
	return 0;
ERR1:
	__free_function_((void **)&server_addr);
	return ret;

}

