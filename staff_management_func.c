#include "staff_management_include.h"

extern int *glo_sfd;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

void *pthread_func(void *arg_v)
{
	PMSG pmsg = (PMSG)arg_v;
	PMSG pmsg_r = (PMSG)__malloc_function_(sizeof(MSG));
	while(1)
	{
		if(The_client_receives_the_message(pmsg_r) == -1)
			break;
		memcpy(pmsg->flags, pmsg_r->flags, sizeof(pmsg->flags));
		memset(pmsg_r, 0, sizeof(MSG));
	}
	kill(getpid(), 9);
	pthread_exit(NULL);
}

int The_client_receives_the_message(PMSG pmsg)//客户端接收信息 
{
	ssize_t ret = 0;
	int flags = 0;
	while(1)
	{
		ret = recv(*glo_sfd, (void *)pmsg, sizeof(MSG), 0);
		if(ret < 0)
		{
			printf("%s %d: recv is error!\n", __func__, __LINE__);
			return -1;
		}
		else if(ret == 0)
		{
			printf("服务端异常!\n");
			return -1;
		}
		printf("%s\n", pmsg->data);
		flags = ((pmsg->flags[1] >> 4) & 0xf);
		if(flags == 0)
		{
			break;
		}
	}
//	if(pmsg->data[strlen(pmsg->data)-1] == '\n')
//		pmsg->data[strlen(pmsg->data)-1] = 0;
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	return 0;
}

int The_client_sends_the_message(PMSG pmsg)//客户端发送消息
{
	int ret  = 0;	
	ret = send(*glo_sfd, (void *)pmsg, sizeof(MSG), 0);
	if(ret < 0)
	{
		printf("%s %d: send error!\n", __func__, __LINE__);
		return ret;
	}
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);
	return 0;
}

int user_connection_operation(PMSG pmsg)//用户连接操作 
{
	int cho = 0;
	cho = user_identity_selection_prompt();//用户身份选择
	switch(cho){
		case 1:
			pmsg->flags[0] &= ~0x3;
			pmsg->flags[0] |= cho;
			The_client_sends_the_message(pmsg);
			Administrator_operation_function(pmsg);//管理员操作函数
			break;
		case 2:
			//printf("普通员工!\n");
			pmsg->flags[0] &= ~0x3;
			pmsg->flags[0] |= cho;
			The_client_sends_the_message(pmsg);
			Client_common_users_operate_functions(pmsg);//客户端普通用户操作函数
			
			break;
		case 3:
			pmsg->flags[0] &= ~0x3;
			pmsg->flags[0] |= cho;
			The_client_sends_the_message(pmsg);
			break;
		default:
			cho = 0;
			pmsg->flags[0] &= ~0x3;
			pmsg->flags[0] |= cho;
			The_client_sends_the_message(pmsg);
			break;

	}
	return cho;
}
int Client_common_users_operate_functions(PMSG pmsg)//客户端普通用户操作函数
{
	int cho = 0;
	while(1)
	{
		pmsg->flags[0] &= ~(0x3 << 2);
		cho = Common_user_operation_prompt_functioni();//_普通用户操作提示函数_
		switch(cho)
		{
			case 1:
				pmsg->flags[0] |= cho << 2;
				The_client_sends_the_message(pmsg);
				Client_common_user_login_operation_function(pmsg);//_客户端普通用户登录操作函数

				break;
			case 2:
				pmsg->flags[0] |= cho << 2;
				The_client_sends_the_message(pmsg);
				break;
			case 3:
				pmsg->flags[0] |= cho << 2;
				The_client_sends_the_message(pmsg);
				break;
			default:
				cho = 0;
				pmsg->flags[0] |= cho << 2;
				The_client_sends_the_message(pmsg);
				break;
		}
		if(cho == 3)
			break;
	}
	return 0;
}

int Client_common_user_login_operation_function(PMSG pmsg)//_客户端普通用户登录操作函数
{
	int flags = 0;
	while(get_user_id(pmsg) != 0);
	while(get_user_passwd(pmsg) != 0);
	The_client_sends_the_message(pmsg);
	flags = (pmsg->flags[0] >> 4) & 0x3;
	if(flags == 2)
	{
		The_client_user_successfully_logs_in_to_implement_the_function(pmsg);//客户端用户登录成功操作_实现函数_

	}
	return 0;
}

int The_client_user_successfully_logs_in_to_implement_the_function(PMSG pmsg)//客户端用户登录成功操作_实现函数_
{
	int flags = 0;
	//xiaopan__
	flags = pmsg->flags[2] & 0x3;
	if(flags == 0)
	{
		while(get_user_passwd(pmsg) != 0);
		while(get_user_name(pmsg) != 0);
		while(get_user_sex(pmsg) != 0);
		while(get_user_telephone(pmsg) != 0);
		while(get_user_addr(pmsg) != 0);
		The_client_sends_the_message(pmsg);
	}
	else if(flags == 1)
	{
		Normal_client_users_use_functions_normally(pmsg);//客户端普通用户正常使用函数
	}
	return 0;
}

int Normal_client_users_use_functions_normally(PMSG pmsg)//客户端普通用户正常使用函数
{
	int cho = 0;
	while(1)
	{
		pmsg->flags[2] &= ~(0x7 << 2);
		cho = A_message_indicating_successful_login_is_displayed(pmsg);//用户登录成功操作提示
		switch(cho)
		{
			case 1:
				pmsg->flags[2] |= cho << 2;
				The_client_sends_the_message(pmsg);
				Common_client_users_modify_their_own_information(pmsg);//客户端普通用户修改自己的信息

				break;
			case 2:
				pmsg->flags[2] |= cho << 2;
				The_client_sends_the_message(pmsg);
				Ordinary_users_view_all_their_information(pmsg);//普通用户查看自己的所有信息
				break;
			case 3:
				pmsg->flags[2] |= cho << 2;
				The_client_sends_the_message(pmsg);
				Common_users_query_the_employee_address_book(pmsg);//普通用户查询员工通讯录
				break;
			case 4:
				pmsg->flags[2] |= cho << 2;
				The_client_sends_the_message(pmsg);
				break;
			default:
				cho = 0;
				pmsg->flags[2] |= cho << 2;
				The_client_sends_the_message(pmsg);
				break;
		}
		if(cho == 4)
			break;
	
	}
	return 0;
}

int Common_users_query_the_employee_address_book(PMSG pmsg)//普通用户查询员工通讯录
{
	printf("正在查询通讯录，请稍等...\n");
	The_client_sends_the_message(pmsg);
	return 0;
}

int Ordinary_users_view_all_their_information(PMSG pmsg)//普通用户查看自己的所有信息
{
	printf("请稍等...\n");
	The_client_sends_the_message(pmsg);
	return 0;
}

int Common_client_users_modify_their_own_information(PMSG pmsg)//客户端普通用户修改自己的信息
{
	int cho = 0;
	while(1)
	{
		pmsg->flags[2] &= ~(0x7 << 5);
		cho = Modification_prompt_for_common_users();//普通用户修改提示_
		switch(cho)
		{
			case 1:
				pmsg->flags[2] |= cho << 5; 
				while(get_user_passwd(pmsg) != 0);
				The_client_sends_the_message(pmsg);
				break;
			case 2:
				pmsg->flags[2] |= cho << 5; 
				while(get_user_name(pmsg) != 0);
				The_client_sends_the_message(pmsg);
				break;
			case 3:
				pmsg->flags[2] |= cho << 5; 
				while(get_user_sex(pmsg) != 0);
				The_client_sends_the_message(pmsg);
				break;
			case 4:
				pmsg->flags[2] |= cho << 5; 
				while(get_user_telephone(pmsg) != 0);
				The_client_sends_the_message(pmsg);
				break;
			case 5:
				pmsg->flags[2] |= cho << 5; 
				while(get_user_addr(pmsg) != 0);
				The_client_sends_the_message(pmsg);
				break;
			case 6:
				pmsg->flags[2] |= cho << 5; 
				The_client_sends_the_message(pmsg);
				break;
			default:
				cho = 0;
				pmsg->flags[2] |= cho << 5; 
				The_client_sends_the_message(pmsg);
				break;
		
		}
		if(cho == 6)
			break;
	}
	return 0;
}

int Modification_prompt_for_common_users()//普通用户修改提示_
{
	int cho = 0;
	printf("==================================================\n");
	printf("============  请选择你要修改的选项  ==============\n");
	printf("============   	1. 密码  	    ==============\n");
	printf("============  	2. 姓名    	    ==============\n");
	printf("============  	3. 性别   	    ==============\n");
	printf("============  	4. 电话             ==============\n");
	printf("============  	5. 地址             ==============\n");
	printf("============  	6. 退出             ==============\n");
	printf("==================================================\n");
	if(scanf("%d", &cho) != 1)
	{
		printf("%s %d: scanf!\n", __func__, __LINE__);
	}
	while(getchar() != '\n');
	return cho;
}

int A_message_indicating_successful_login_is_displayed(PMSG pmsg)//用户登录成功操作提示
{
	int cho = 0;
	printf("================================================\n");
	printf("============  请选择你要做的操作  ==============\n");
	printf("============  1. 修改自己的信息   ==============\n");
	printf("============  2. 查看自己的信息   ==============\n");
	printf("============  3. 查看公司通讯录   ==============\n");
	printf("============  4. 退出             ==============\n");
	printf("================================================\n");
	if(scanf("%d", &cho) != 1)
	{
		printf("%s %d: scanf!\n", __func__, __LINE__);
	}
	while(getchar() != '\n');
	return cho;
}


int Common_user_operation_prompt_functioni()//_普通用户操作提示函数_
{
	int ret = 0, cho = 0;
	printf("=============================================\n");
	printf("============= 请选择你要做的操作 ============\n");
	printf("=============  1. 登录           ============\n");
	printf("=============  2. 注册(不可用)   ============\n");
	printf("=============  3. 退出           ============\n");
	printf("=============================================\n");
	ret = scanf("%d", &cho);
	if(ret != 1)
	{
		printf("%s %d: scanf!\n", __func__, __LINE__);
	}
	while(getchar() != '\n');
	return cho;
}

int Administrator_operation_function(PMSG pmsg)//管理员操作函数
{
	int cho = 0;
	while(1)
	{
		cho = 0;
		pmsg->flags[0] &= ~(0x3 << 2);
		cho = Administrator_Operation_Tips();
		switch(cho)
		{
			case 1://注册
				pmsg->flags[0] |= cho << 2;
				The_client_sends_the_message(pmsg);
				The_administrator_registers_the_account_function(pmsg);//管理员注册账号函数_
				break;
			case 2://登录
				pmsg->flags[0] |= cho << 2;
				The_client_sends_the_message(pmsg);
				Administrator_login_account_function(pmsg);//管理员登录账号函数
				
				break;
			case 3://退出
				pmsg->flags[0] |= cho << 2;
				The_client_sends_the_message(pmsg);
				break;
			default:
				cho = 0;
				pmsg->flags[0] |= cho << 2;
				The_client_sends_the_message(pmsg);
				break;
		}
		if(cho == 3)
			break;
	}
	return 0;
}


int Administrator_login_account_function(PMSG pmsg)//管理员登录账号函数
{
	while(get_user_id(pmsg) != 0);
	while(get_user_passwd(pmsg) != 0);
	The_client_sends_the_message(pmsg);
	if(((pmsg->flags[0] >> 0x4) & 0x3) == 0x2)
	{
		The_administrator_successfully_logs_in_to_the_function(pmsg);//管理员登录成功操作函数

	}	
	return 0;
}

int The_administrator_successfully_logs_in_to_the_function(PMSG pmsg)//管理员登录成功操作函数
{
	int cho = 0;
	while(1)
	{
		cho = Prompt_function_for_administrator_login_operation(pmsg);//管理员登录操作提示函数
		switch(cho)
		{
			case 1:
				pmsg->flags[1] &= ~(0xf);
				pmsg->flags[1] |= cho;
				The_client_sends_the_message(pmsg);
				Add_employee_information_manipulation_functioni(pmsg);//增加员工信息操作函数
				break;
			case 2:
				pmsg->flags[1] &= ~(0xf);
				pmsg->flags[1] |= cho;
				The_client_sends_the_message(pmsg);
				Delete_employee_account_information_operation_function(pmsg);//删除员工账户信息操作函数
				break;
			case 3:
				pmsg->flags[1] &= ~(0xf);
				pmsg->flags[1] |= cho;
				The_client_sends_the_message(pmsg);
				Client_user_modifies_information_operation_function(pmsg);//客户端用户修改信息操作函数
				break;
			case 4:
				pmsg->flags[1] &= ~(0xf);
				pmsg->flags[1] |= cho;
				The_client_sends_the_message(pmsg);
				Example_Modify_the_account_information_of_a_user(pmsg);//修改用户自己的账号信息
				break;
			case 5:
				pmsg->flags[1] &= ~(0xf);
				pmsg->flags[1] |= cho;
				The_client_sends_the_message(pmsg);
				The_administrator_queries_information_about_an_employee(pmsg);//管理员查询单个员工信息_
				break;
			case 6:
				pmsg->flags[1] &= ~(0xf);
				pmsg->flags[1] |= cho;
				The_client_sends_the_message(pmsg);
				The_administrator_queries_information_about_all_employees(pmsg);//管理员查询所有员工信息
				break;
			case 7:
				pmsg->flags[1] &= ~(0xf);
				pmsg->flags[1] |= cho;
				The_client_sends_the_message(pmsg);
				break;
			default:
				cho = 0;
				pmsg->flags[1] &= ~(0xf);
				pmsg->flags[1] |= cho;
				The_client_sends_the_message(pmsg);
				break;
		}
		if(cho == 7)
			break;
	}
	return 0;
}

int The_administrator_queries_information_about_all_employees(PMSG pmsg)//管理员查询所有员工信息
{
	printf("请稍等...\n");
	The_client_sends_the_message(pmsg);
	return 0;
}

int The_administrator_queries_information_about_an_employee(PMSG pmsg)//管理员查询单个员工信息_
{
	while(get_user_id(pmsg) != 0);
	The_client_sends_the_message(pmsg);
	return 0;
}

int Example_Modify_the_account_information_of_a_user(PMSG pmsg)//修改用户自己的账号信息
{
	while(get_user_passwd(pmsg) != 0);
	The_client_sends_the_message(pmsg);
	return 0;
}


int Funcddtion_used_by_client_users_to_obtain_user_information(PMSG pmsg)//客户端用户获取用户信息操作函数_
{
	pmsg->flags[0] &= ~(0x3 << 6);
	pmsg->flags[0] |= (0x1 << 6);
	pmsg->flags[1] &= ~(0xf << 4);
	The_client_sends_the_message(pmsg);
	
//reminder_
	return 0;
}

int Client_user_modifies_information_operation_function(PMSG pmsg)//客户端用户修改信息操作函数
{
//	Funcddtion_used_by_client_users_to_obtain_user_information(pmsg);//客户端用户获取用户信息操作函数_
	while(get_user_id(pmsg) != 0);
	while(get_user_passwd(pmsg) != 0);
	while(get_user_name(pmsg) != 0);
	while(get_user_sex(pmsg) != 0);
	while(get_user_wage(pmsg) != 0);
	while(get_user_telephone(pmsg) != 0);
	while(get_user_addr(pmsg) != 0);
	The_client_sends_the_message(pmsg);
	return 0;
}

int Delete_employee_account_information_operation_function(PMSG pmsg)//删除员工账户信息操作函数
{
	//xiaopan	
	while(get_user_id(pmsg) != 0);
	The_client_sends_the_message(pmsg);
	return 0;
}

int Add_employee_information_manipulation_functioni(PMSG pmsg)//增加员工信息操作函数
{
	while(get_user_id(pmsg) != 0);
	while(get_user_passwd(pmsg) != 0);
	while(get_user_name(pmsg) != 0);
	while(get_user_wage(pmsg) != 0);
	The_client_sends_the_message(pmsg);
	return 0;
}


int Prompt_function_for_administrator_login_operation(PMSG pmsg)//管理员登录操作提示函数
{
	int cho = 0;
	printf("====================================================\n");
	printf("===============  请选择你需要做的操作  =============\n");
	printf("===============  1. 增加员工账户信息   =============\n");
	printf("===============  2. 删除员工账户信息   =============\n");
	printf("===============  3. 修改员工账户信息   =============\n");
	printf("===============  4. 修改自己账号密码   =============\n");
	printf("===============  5. 查询单个员工信息   =============\n");
	printf("===============  6. 查询所有员工信息   =============\n");
	printf("===============  7. 退出               =============\n");
	printf("====================================================\n");	
	int ret = scanf("%d", &cho);
	if(ret != 1)
	{
		printf("%s %d: scanf error!\n", __func__, __LINE__);
		while(getchar() != '\n');
		return -1;
	}
	while(getchar() != '\n');
	return cho;
}


int The_administrator_registers_the_account_function(PMSG pmsg)//管理员注册账号函数_
{
	while(get_user_id(pmsg) != 0);
	while(get_user_passwd(pmsg) != 0);
	while(get_user_name(pmsg) != 0);
	The_client_sends_the_message(pmsg);
	return 0;
}

int get_user_id(PMSG pmsg)
{
	unsigned int id = 0, ret;
	printf("请输入账号: \n");
	ret = scanf("%d", &id);
	if(ret != 1)
	{
		printf("%s %d: scanf\n", __func__, __LINE__);
		while(getchar() != '\n');
		return -1;
	}
	while(getchar() != '\n');
	ret = sprintf(pmsg->id, "%d", id);
	if(ret < 0)
	{
		printf("%s %d: sprintf is error!\n", __func__, __LINE__);
		return -1;
	}
	return 0;
}

int get_user_name(PMSG pmsg)
{
	char *p = NULL;
	printf("请输入姓名: \n");
	p = fgets(pmsg->name, sizeof(pmsg->name), stdin);
	if(p == NULL)
	{
		printf("%s %d: fgets is error!\n", __func__, __LINE__);
		return -1;
	}
	pmsg->name[strlen(pmsg->name)-1] = 0;
	return 0;
}
int get_user_sex(PMSG pmsg)
{
	int ret = 0;
	char ch = 0;
	printf("请输入性别: 男:1 女:0\n");
	ret = scanf("%hhd", &ch);
	if(ret != 1)
	{
		printf("%s %d: scanf is error!\n", __func__, __LINE__);
		while(getchar() != '\n');
		return -1;
	}
	while(getchar() != '\n');
	if(!(ch == 0 || ch == 1))
		return -1;
	pmsg->sex = ch;
	return 0;
}

int get_user_passwd(PMSG pmsg)
{
	char *pret = 0;
	printf("请输入密码: \n");
	pret = fgets(pmsg->passwd, sizeof(pmsg->passwd), stdin);
	if(pret == NULL)
	{
		printf("%s %d: fgets is error!\n", __func__, __LINE__);
		return -1;
	}
	pmsg->passwd[strlen(pmsg->passwd)-1] = 0;
	return 0;
}
int get_user_addr(PMSG pmsg)
{
	char *pret = 0;
	printf("请输入住址: \n");
	pret = fgets(pmsg->addr, sizeof(pmsg->addr), stdin);
	if(pret == NULL)
	{
		printf("%s %d: fgets is error!\n", __func__, __LINE__);
		return -1;
	}
	pmsg->addr[strlen(pmsg->addr)-1] = 0;
	
	return 0;
}

int get_user_wage(PMSG pmsg)
{
	int ret = 0;
	float wage = 0;
	printf("请输入薪资:\n");
	ret = scanf("%f", &wage);
	if(ret != 1)
	{
		printf("%s %d: scanf is error!\n", __func__, __LINE__);
		while(getchar() != '\n');
		return -1;
	}
	while(getchar() != '\n');
	ret = sprintf(pmsg->wage, "%.2f", wage);
	if(ret < 0)
	{
		printf("%s %d: sprintf is error!\n", __func__, __LINE__);
		return -1;
	}
	return 0;
}
int get_user_telephone(PMSG pmsg)
{
	int ret =0;
	long int tel = 0;
	printf("请输入电话号:\n");
	ret = scanf("%ld", &tel);
	if(ret != 1)
	{
		printf("%s %d: sprintf is error!\n", __func__, __LINE__);
		while(getchar() != '\n');
		return -1;
	}
	while(getchar() != '\n');
	ret = sprintf(pmsg->telephone, "%ld", tel);
	if(ret < 0)
	{
		printf("%s %d: sprintf is error!\n", __func__, __LINE__);
		return -1;
	}
	return 0;
}


int get_user_data(PMSG pmsg)
{
	char *pret = 0;
	printf("请输入你要输入的内容: \n");
	pret = fgets(pmsg->data, sizeof(pmsg->data), stdin);
	if(pret == NULL)
	{
		printf("%s %d: fgets is error!\n", __func__, __LINE__);
		return -1;
	}
	pmsg->data[strlen(pmsg->data)-1] = 0;
	return 0;
}





int Administrator_Operation_Tips()//管理员操作提示 
{
	int cho = 0, ret;
	printf("=============================================\n");
	printf("=============== 请选择你要做的操作 ==========\n");
	printf("===============  1. 注册           ==========\n");
	printf("===============  2. 登录           ==========\n");
	printf("===============  3. 退出           ==========\n");
	printf("=============================================\n");
	ret = scanf("%d", &cho);
	if(ret != 1)
	{
		printf("%s %d: scanf is error!\n", __func__, __LINE__);
		while(getchar() != '\n');
		return -1;
	}
	while(getchar() != '\n');
	return cho;
}

int user_identity_selection_prompt()//用户身份选择
{
	int cho = 0, ret;
	printf("================================================\n");
	printf("===============  请选择你的身份 ================\n");
	printf("===============  1. 管理员      ================\n");
	printf("===============  2. 普通员工    ================\n");
	printf("===============  3. 退出        ================\n");
	printf("================================================\n");
	ret = scanf("%d", &cho);
	if(ret != 1)
	{
		printf("%s %d: scanf is error!\n", __func__, __LINE__);
		while(getchar() != '\n');
		return -1;
	}
	while(getchar() != '\n');
	return cho;
}



void *__malloc_function_(size_t size)
{
	void *p = malloc(size);
	if(p == NULL)
	{
		printf("%s_%d: malloc is error!\n", __func__, __LINE__);
		return NULL;
	}
	memset(p, 0, size);
	return p;
}

void *__free_function_(void **p)
{
	free(*p);
	*p = NULL;
	return NULL;
}


