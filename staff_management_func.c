#include "staff_management_include.h"


int The_server_parses_client_operations(PMSG pmsg, int *newfd)//服务器解析客户端操作 
{
	int ret = 0;
	ret = Service_user_authentication_function(pmsg, newfd);//服务用户身份验证函数_

	return ret;
}

int Service_user_authentication_function(PMSG pmsg, int *newfd)//服务用户身份验证函数_
{
	char cho = 0;
	The_server_receives_information_from_the_client(pmsg, newfd);
	cho = pmsg->flags[0] & 0x3;
	switch(cho)
	{
		case 1:
			memset(pmsg->data, 0, sizeof(pmsg->data));
			strcpy(pmsg->data, "操作员!");
			The_server_sends_information_to_the_client(pmsg, newfd);
			Server_administrator_operation_resolution(pmsg, newfd);//服务器管理员操作解析_
			break;
		case 2:
			memset(pmsg->data, 0, sizeof(pmsg->data));
			strcpy(pmsg->data, "普通职工!");
			The_server_sends_information_to_the_client(pmsg, newfd);
			The_server_resolves_common_employee_operations(pmsg, newfd);//服务器解析普通职员操作
			break;
		case 3:
			memset(pmsg->data, 0, sizeof(pmsg->data));
			strcpy(pmsg->data, "退出!");
			The_server_sends_information_to_the_client(pmsg, newfd);
			break;
		default:
			memset(pmsg->data, 0, sizeof(pmsg->data));
			strcpy(pmsg->data, "指令错误!");
			The_server_sends_information_to_the_client(pmsg, newfd);
			break;
	}
	return cho;
}

int The_server_resolves_common_employee_operations(PMSG pmsg, int *newfd)//服务器解析普通职员操作
{
	int cho = 0;
	while(1)
	{
		The_server_receives_information_from_the_client(pmsg, newfd);
		cho = (pmsg->flags[0] >> 2) & 0x3;
		switch(cho)
		{
			case 1:
				strcpy(pmsg->data, "登录!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				The_server_resolves_the_common_user_login_operation_function(pmsg, newfd);//服务端解析普通用户登录操作函数_
				break;
			case 2://暂时不可用
				strcpy(pmsg->data, "注册!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			case 3:
				strcpy(pmsg->data, "退出!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			default:
				strcpy(pmsg->data, "错误!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
		}
		if(cho == 3)
			break;
	}
	return 0;
}

int The_server_resolves_the_common_user_login_operation_function(PMSG pmsg, int *newfd)//服务端解析普通用户登录操作函数_
{
	int ret = 0;
	The_server_receives_information_from_the_client(pmsg, newfd);
	ret = A_normal_user_validates_the_login_function(pmsg, newfd);//普通用户验证登录函数
	if(ret == 2)
	{
		ret =  Verify_whether_this_is_the_user_first_login(pmsg, newfd);
		if(ret == 0)//验证是否是用户首次登录
		{
			pmsg->flags[2] &= ~(0x3);
			pmsg->flags[2] |= 0;
			memset(pmsg->data, 0, sizeof(pmsg->data));
			strcpy(pmsg->data, "登录成功，请你先完善你的信息!");
			The_server_sends_information_to_the_client(pmsg, newfd);
			The_server_improves_the_user_information_function(pmsg, newfd);//服务器完善用户信息函数_
			memset(pmsg->data, 0, sizeof(pmsg->data));
			strcpy(pmsg->data, "信息更新完成，请你重新登录!");
			The_server_sends_information_to_the_client(pmsg, newfd);
		}
		else 
		{
			pmsg->flags[2] &= ~(0x3);
			pmsg->flags[2] |= 1;
			memset(pmsg->data, 0, sizeof(pmsg->data));
			strcpy(pmsg->data, "登录成功，欢迎使用!");
			The_server_sends_information_to_the_client(pmsg, newfd);
			The_server_parses_the_user_login_operation_function(pmsg, newfd);//服务端解析用户登录操作函数
		}
	}
	else
	{
		The_server_sends_information_to_the_client(pmsg, newfd);
	}
	return 0;
}



int The_server_parses_the_user_login_operation_function(PMSG pmsg, int *newfd)//服务端解析用户登录操作函数
{
	char cho = 0;
	while(1)
	{
		The_server_receives_information_from_the_client(pmsg, newfd);
		memset(pmsg->data, 0, sizeof(pmsg->data));
		cho = (pmsg->flags[2] >> 2) & 0x7;
		switch(cho)
		{
			case 1:
				strcpy(pmsg->data, "修改信息!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				The_server_parses_the_modification_information_of_common_employees(pmsg, newfd);//服务端解析普通员工修改信息
				break;
			case 2:
				strcpy(pmsg->data, "查看信息!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				The_server_parses_the_client_to_query_its_own_account_information(pmsg, newfd);//服务端解析客户查询自己账号信息

				break;
			case 3:
				strcpy(pmsg->data, "查看通讯录!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				The_server_resolves_the_client_query_address_book(pmsg, newfd);//服务端解析客户端查询通讯录
				break;
			case 4:
				strcpy(pmsg->data, "退出!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			default:
				strcpy(pmsg->data, "指令错误!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
		}
		if(cho == 4)
			break;
	}
	return 0;
}
int The_server_resolves_the_client_query_address_book(PMSG pmsg, int *newfd)//服务端解析客户端查询通讯录
{
	int ret = 0;
	int  i = 0;
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(MB);
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	The_server_receives_information_from_the_client(pmsg, newfd);
	db = __sqlite3_open_func_(TAB_NAME);
	ret = sprintf(tab->cmd, "select name, sex, telephone from staff_info_tab");
	if(ret > 0)
	{
		__sqlite3_get_table_func_(db, tab);
		for(i = 1; i <= tab->row; i++)
		{
			if(i != 1)
			{
				pmsg->flags[1] |= 0x1 << 4;
				memset(pmsg->data, 0, sizeof(pmsg->data));
				strcpy(pmsg->data, buf);
				The_server_sends_information_to_the_client(pmsg, newfd);
				memset(buf, 0, MB);
			}
			sprintf(buf, "name: %-12s sex: %s telephone: %12s", \
 tab->result[tab->column*i+0], tab->result[tab->column*i+1], tab->result[tab->column*i+2]);
		}
	}
	pmsg->flags[1] &= ~(0xf << 4);
	memset(pmsg->data, 0, sizeof(pmsg->data));
	strcpy(pmsg->data, buf);
	The_server_sends_information_to_the_client(pmsg, newfd);
	__sqlite3_close_func_(db);
	sqlite3_free_table(tab->result);
	__free_function((void **)&tab->cmd);
	__free_function((void **)&tab);
	__free_function((void **)&buf);
	return 0;	
}

int The_server_parses_the_client_to_query_its_own_account_information(PMSG pmsg, int *newfd)//服务端解析客户查询自己账号信息
{
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(MB);
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	The_server_receives_information_from_the_client(pmsg, newfd);
	db = __sqlite3_open_func_(TAB_NAME);
	sprintf(tab->cmd, "select * from staff_info_tab where id=%s;", pmsg->id);
	__sqlite3_get_table_func_(db, tab);
	sprintf(buf, "id: %-12s passwd: %-12s name: %-12s sex: %s addr: %-12s wage: %-12s telephone: %-12s",\
 tab->result[tab->column+0], tab->result[tab->column+1], tab->result[tab->column+2], \
 tab->result[tab->column+3], tab->result[tab->column+4], tab->result[tab->column+5], \
 tab->result[tab->column+6]);	
	memset(pmsg->data, 0, sizeof(pmsg->data));
	strcpy(pmsg->data, buf);
	The_server_sends_information_to_the_client(pmsg, newfd);
	__sqlite3_close_func_(db);
	sqlite3_free_table(tab->result);
	__free_function((void **)&tab->cmd);
	__free_function((void **)&tab);
	__free_function((void **)&buf);
	return 0;
}

int The_server_parses_the_modification_information_of_common_employees(PMSG pmsg, int *newfd)//服务端解析普通员工修改信息
{
	int cho = 0;
	sqlite3 *db = NULL;
	db = __sqlite3_open_func_(TAB_NAME);
	char *buf = (char *)__malloc_function_(SIZE);
	while(1)
	{
		The_server_receives_information_from_the_client(pmsg, newfd);
		memset(pmsg->data, 0, sizeof(pmsg->data));
		memset(buf, 0, SIZE);
		cho = (pmsg->flags[2] >> 5) & 0x7;
		switch(cho)
		{
			case 1:
				sprintf(buf, "update staff_info_tab set passwd=\"%s\" where id=%s;", pmsg->passwd, pmsg->id);
				__sqlite3_exec_function_(db, buf);
				strcpy(pmsg->data, "密码修改成功!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			case 2:
				sprintf(buf, "update staff_info_tab set name=\"%s\" where id=%s;", pmsg->name, pmsg->id);
				__sqlite3_exec_function_(db, buf);
				strcpy(pmsg->data, "名字修改成功!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			case 3:
				sprintf(buf, "update staff_info_tab set passwd=%d where id=%s;", pmsg->sex, pmsg->id);
				__sqlite3_exec_function_(db, buf);
				strcpy(pmsg->data, "性别修改成功!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			case 4:
				sprintf(buf, "update staff_info_tab set telephone=%s where id=%s;", pmsg->telephone, pmsg->id);
				__sqlite3_exec_function_(db, buf);
				strcpy(pmsg->data, "电话号码修改成功!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			case 5:
				sprintf(buf, "update staff_info_tab set addr=\"%s\" where id=%s;", pmsg->addr, pmsg->id);
				__sqlite3_exec_function_(db, buf);
				strcpy(pmsg->data, "地址修改成功!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			case 6:
				strcpy(pmsg->data, "退出!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			default:
				strcpy(pmsg->data, "指令错误!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
		}
		if(cho == 6)
			break;

	}
	__free_function((void **)&buf);
	__sqlite3_close_func_(db);
	return 0;
}

int The_server_improves_the_user_information_function(PMSG pmsg, int *newfd)//服务器完善用户信息函数_
{
	sqlite3 *db = NULL;
	int ret = 0;
	char *buf = (char *)__malloc_function_(SIZE);
	The_server_receives_information_from_the_client(pmsg, newfd);
	db = __sqlite3_open_func_(TAB_NAME);
	ret = sprintf(buf, "update staff_info_tab set passwd=\"%s\" where id=%s;", pmsg->passwd, pmsg->id);
	if(ret < 0)
	{
		printf("%s %d: sprintf\n", __func__, __LINE__);
	}
	else
	{
		puts(buf);//xiaopan
		__sqlite3_exec_function_(db, buf);
	}
	memset(buf, 0, SIZE);
	sprintf(buf, "update staff_info_tab set name=\"%s\" where id=%s;", pmsg->name, pmsg->id);
	__sqlite3_exec_function_(db, buf);
	memset(buf, 0, SIZE);
	sprintf(buf, "update staff_info_tab set sex=%d where id=%s;", pmsg->sex, pmsg->id);
	__sqlite3_exec_function_(db, buf);
	memset(buf, 0, SIZE);
	sprintf(buf, "update staff_info_tab set telephone=%s where id=%s;", pmsg->telephone, pmsg->id);
	__sqlite3_exec_function_(db, buf);
	memset(buf, 0, SIZE);
	sprintf(buf, "update staff_info_tab set addr=\"%s\" where id=%s;", pmsg->addr, pmsg->id);
	__sqlite3_exec_function_(db, buf);
	__sqlite3_close_func_(db);
	__free_function((void **)&buf);
	return 0;
}

int Verify_whether_this_is_the_user_first_login(PMSG pmsg, int *newfd)//验证是否是用户首次登录
{
	int ret = 0;
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(SIZE);
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	ret = sprintf(tab->cmd, "select statas from staff_info_tab where id=%s;", pmsg->id);
	if(ret < 0)
	{
		printf("%s %d: sprintf!\n", __func__, __LINE__);
	}
	else
	{
		db = __sqlite3_open_func_(TAB_NAME);
		__sqlite3_get_table_func_(db, tab);
		if(atoi(tab->result[1]) == 16)
		{
			sprintf(buf, "update staff_info_tab set statas=%d where id=%s;", 0, pmsg->id);
			__sqlite3_exec_function_(db, buf);
			ret = 0;
		}
		else
		{
			sprintf(buf, "update staff_info_tab set statas=%d where id=%s;", 1, pmsg->id);
			__sqlite3_exec_function_(db, buf);
			ret = 1;
		}
		sqlite3_free_table(tab->result);
		__sqlite3_close_func_(db);

	}
	__free_function((void **)&tab->cmd);
	__free_function((void **)&tab);
	return ret;
}

int A_normal_user_validates_the_login_function(PMSG pmsg, int *newfd)//普通用户验证登录函数
{
	int ret = 0, i = 0, flags = 0;
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(MB);
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	ret = sprintf(tab->cmd, "select id, passwd, statas from staff_info_tab;");
	if(ret < 0)
	{
		printf("%s %d: sprintf!\n", __func__, __LINE__);
	}
	db = __sqlite3_open_func_(TAB_NAME);
	__sqlite3_get_table_func_(db, tab);
	for(i = 1; i <= tab->row; i++)
	{
		if(strcmp(tab->result[i*tab->column], pmsg->id) == 0)
		{
			flags = 1;
			if(strcmp(tab->result[i*tab->column+1], pmsg->passwd) == 0)
			{
				printf("===%s===%s===", tab->result[i*tab->column+2], "1");
				if(strcmp(tab->result[i*tab->column+2], "1") == 0)
				{
					flags = 3;
				}
				else
					flags = 2;
			}
		}
	}
	memset(pmsg->data, 0, sizeof(pmsg->data));
	pmsg->flags[0] &= ~(0x3 << 4);
	switch(flags)
	{
		case 0:
			pmsg->flags[0] |= flags << 4;
			strcpy(pmsg->data, "账户不存在!");
			break;
		case 1:
			pmsg->flags[0] |= flags << 4;
			strcpy(pmsg->data, "密码错误!");
			break;
		case 2:
			pmsg->flags[0] |= flags << 4;
			strcpy(pmsg->data, "登录成功!");
			break;
		case 3:
			pmsg->flags[0] |= flags << 4;
			strcpy(pmsg->data, "账户已经登录!");
			break;

	}
	__sqlite3_close_func_(db);
	sqlite3_free_table(tab->result);
	__free_function((void **)&tab->cmd);
	__free_function((void **)&tab);
	__free_function((void **)&buf);
	return flags;
}

int Server_administrator_operation_resolution(PMSG pmsg, int *newfd)//服务器管理员操作解析_
{
	char cho = 0;
	while(1)
	{
		The_server_receives_information_from_the_client(pmsg, newfd);
		cho = (pmsg->flags[0] & (0x3 << 2)) >> 2;
		memset(pmsg->data, 0, sizeof(pmsg->data));
		switch(cho)
		{
			case 1://注册
				strcpy(pmsg->data, "注册!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				Server_administrator_registration_information_manipulation_function(pmsg, newfd);//服务器管理员注册信息操作函数


				break;
			case 2://登录
				strcpy(pmsg->data, "登录!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				Server_administrator_login_information_operation_function(pmsg, newfd);//服务器管理员登录信息操作函数
				break;
			case 3://退出
				strcpy(pmsg->data, "退出!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			default:
				strcpy(pmsg->data, "错误!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;

		}
		if(cho == 3)
			break;
	}
	memset(pmsg->data, 0, sizeof(pmsg->data));
	return 0;
}

int Server_administrator_login_information_operation_function(PMSG pmsg, int *newfd)//服务器管理员登录信息操作函数
{
	int ret = 0;
	The_server_receives_information_from_the_client(pmsg, newfd);
	ret = Account_password_Login_information_authentication_function(pmsg, newfd);//账号密码登录信息验证函数
	if(ret == 2)		
	{
		The_administrator_successfully_logs_in_to_the_function(pmsg, newfd);//管理员登录成功操作函数
	}
	return 0;
}

int The_administrator_successfully_logs_in_to_the_function(PMSG pmsg, int *newfd)//管理员登录成功操作函数
{
	int cho = 0;
	while(1)	
	{
		The_server_receives_information_from_the_client(pmsg, newfd);
		cho = pmsg->flags[1] & 0xf;
		memset(pmsg->data, 0, sizeof(pmsg->data));
		switch(cho)
		{
			case 1:
				strcpy(pmsg->data, "增加员工信息");
				The_server_sends_information_to_the_client(pmsg, newfd);
				The_server_parses_the_client_operation_functions(pmsg, newfd);//服务器解析客户端操作函数
				break;
			case 2:
				strcpy(pmsg->data, "删除员工信息");
				The_server_sends_information_to_the_client(pmsg, newfd);
				The_server_parses_the_client_delete_account_function(pmsg,newfd);//服务端解析客户端删除账户函数

				break;
			case 3:
				strcpy(pmsg->data, "修改员工信息");
				The_server_sends_information_to_the_client(pmsg, newfd);
				The_server_resolves_the_client_and_modifies_the_user_account_information(pmsg, newfd);//服务端解析客户端修改用户账号信息

				break;
			case 4:
				strcpy(pmsg->data, "修改账户密码");
				The_server_sends_information_to_the_client(pmsg, newfd);
				The_server_resolves_the_administrator_to_change_the_account_password(pmsg, newfd);//服务端解析管理员用户修改账户密码
				break;
			case 5:
				strcpy(pmsg->data, "查询单个员工信息");//管理员查询单个员工信息 
				The_server_sends_information_to_the_client(pmsg, newfd);
				Server_resolution_The_administrator_queries_information_about_a_single_employee(pmsg, newfd);//服务器解析管理员查询单个员工信息_


				break;
			case 6:
				strcpy(pmsg->data, "查询所有员工信息");
				The_server_sends_information_to_the_client(pmsg, newfd);
				Server_administrator_view_all_employee_information(pmsg, newfd);//服务器管理员查看所有职工信息
				break;
			case 7:
				strcpy(pmsg->data, "退出!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
			default:
				strcpy(pmsg->data, "指令错误!");
				The_server_sends_information_to_the_client(pmsg, newfd);
				break;
		}
		if(cho == 7)
			break;
	}
	memset(pmsg->data, 0, sizeof(pmsg->data));
	return 0;
}

int Server_administrator_view_all_employee_information(PMSG pmsg, int *newfd)//服务器管理员查看所有职工信息
{
	int ret = 0, i, len;
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(MB);
	pmsg->flags[1] &= ~(0xf << 4);
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	The_server_receives_information_from_the_client(pmsg, newfd);
	db = __sqlite3_open_func_(TAB_NAME);
	ret = sprintf(tab->cmd, "select * from staff_info_tab;");
	if(ret < 0)
	{
		printf("%s %d: error!\n", __func__, __LINE__);
	}
	else
	{
		puts(tab->cmd);//xiaopan
		__sqlite3_get_table_func_(db, tab);
		memset(tab->cmd, 0, SIZE);
		printf("%d %d\n", tab->row, tab->column);//xiaopan
		for(i = 1; i <= tab->row;i++)
		{
			memset(pmsg->data, 0, sizeof(pmsg->data));
			strcpy(pmsg->data, buf);
			pmsg->flags[1] += 0x1 << 4;
			The_server_sends_information_to_the_client(pmsg, newfd);
			memset(buf, 0, SIZE);
			len = sprintf(buf, "id: %-12s passwd: %-12s name: %-12s sex: %-6s addr: %-12s wage: %-12s$ telephone: %-12s statas: %s",\
 tab->result[i*tab->column+0], tab->result[i*tab->column+1], tab->result[i*tab->column+2],\
 tab->result[i*tab->column+3], tab->result[i*tab->column+4], tab->result[i*tab->column+5],\
 tab->result[i*tab->column+6], tab->result[i*tab->column+7]);
			if(len < 0)
			{
				printf("%s %d: sprintf!\n", __func__, __LINE__);
			}
		}
	}
	memset(pmsg->data, 0, sizeof(pmsg->data));
	strcpy(pmsg->data, buf);
	pmsg->flags[1] &= ~(0xf << 4);
	The_server_sends_information_to_the_client(pmsg, newfd);
	memset(buf, 0, SIZE);
	__sqlite3_close_func_(db);
	sqlite3_free_table(tab->result);
	__free_function((void **)&tab->cmd);
	__free_function((void **)&tab);
	__free_function((void **)&buf);
	return 0;


	return 0;
}

int Server_resolution_The_administrator_queries_information_about_a_single_employee(PMSG pmsg, int *newfd)//服务器解析管理员查询单个员工信息_
{
	int ret = 0, i;
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(MB);
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	The_server_receives_information_from_the_client(pmsg, newfd);
	db = __sqlite3_open_func_(TAB_NAME);
	ret = sprintf(tab->cmd, "select * from staff_info_tab where id=%s;", pmsg->id);
	if(ret < 0)
	{
		printf("%s %d: error!\n", __func__, __LINE__);
	}
	else
	{
		puts(tab->cmd);
		__sqlite3_get_table_func_(db, tab);
		memset(tab->cmd, 0, SIZE);
		for(i = 1; i <= tab->row;i++)
		{
			if(strcmp(tab->result[i*tab->column], pmsg->id) == 0)
			{
				memset(buf, 0, SIZE);
				sprintf(buf, "id: %-12s passwd: %-12s name: %-12s sex: %s addr: %-12s wage: %-12s$ telephone: %-12s statas: %s",\
 tab->result[i*tab->column+0], tab->result[i*tab->column+1], tab->result[i*tab->column+2],\
 tab->result[i*tab->column+3], tab->result[i*tab->column+4], tab->result[i*tab->column+5],\
 tab->result[i*tab->column+6], tab->result[i*tab->column+7]);
				memset(pmsg->data, 0, sizeof(pmsg->data));
				strcpy(pmsg->data, buf);
			}
		}
	}
	
	The_server_sends_information_to_the_client(pmsg, newfd);
	sqlite3_free_table(tab->result);
	__sqlite3_close_func_(db);
	__free_function((void **)&tab->cmd);
	__free_function((void **)&tab);
	__free_function((void **)&buf);
	return 0;
}



int The_server_resolves_the_administrator_to_change_the_account_password(PMSG pmsg, int *newfd)//服务端解析管理员用户修改账户密码
{
	int ret= 0;
	sqlite3 *db = NULL;
	char *cmd = (char *)__malloc_function_(SIZE);
	The_server_receives_information_from_the_client(pmsg, newfd);
	db = __sqlite3_open_func_(TAB_NAME);
	ret = sprintf(cmd, "update admin_table set passwd=\"%s\" where id=\"%s\";", pmsg->passwd, pmsg->id);
	if(ret < 0)
	{
		printf("%s %d: error!\n", __func__, __LINE__);
	}
	__sqlite3_exec_function_(db, cmd);
	__sqlite3_close_func_(db);
	__free_function((void **)&cmd);
	The_server_sends_information_to_the_client(pmsg, newfd);
	return 0;
}

int The_server_obtains_the_common_employee_information_table(PMSG pmsg, int *newfd)//服务端获取普通员工信息表_
{
	int ret = 0, i, j;
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(MB);
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	strcpy(tab->cmd, "select * from staff_info_tab");
	db = __sqlite3_open_func_(TAB_NAME);
	__sqlite3_get_table_func_(db, tab);
	for(i = 0; i <= tab->row; i++)
	{
		for(j = 0; j < tab->column; j++)
		{
			if(ret >= MB-20)
			{
				memset(pmsg->data, 0, sizeof(pmsg->data));
				strcpy(pmsg->data, buf);
				puts(buf);
				pmsg->flags[1] += 0x1 << 4;
				The_server_sends_information_to_the_client(pmsg, newfd);
				memset(buf, 0, sizeof(MB));
				ret = 0;
			}
			else if (ret >= MB-12)
			{
				memset(pmsg->data, 0, sizeof(pmsg->data));
				strcpy(pmsg->data, buf);
				pmsg->flags[1] += 0x1 << 4;
				The_server_sends_information_to_the_client(pmsg, newfd);
				memset(buf, 0, sizeof(MB));
				ret = 0;
			}
			if(j != 4)
			{
				if(tab->result[i*tab->column+j] != 0)
					ret = sprintf(buf+ret, "%10s ", tab->result[i*tab->column+j]);
				else
					ret = sprintf(buf+ret, "%10s ", " ");
			}
			else
			{
				if(tab->result[i*tab->column+j] != 0)
					ret = sprintf(buf+ret, "%10s ", tab->result[i*tab->column+j]);
				else
					ret = sprintf(buf+ret, "%10s ", " ");
			}
		}
		ret = sprintf(buf+ret, "\n");
			
	}
	printf("%s\n", buf);
	memset(pmsg->data, 0, sizeof(pmsg->data));
	strcpy(pmsg->data, buf);
	The_server_sends_information_to_the_client(pmsg, newfd);
	__sqlite3_close_func_(db);
	sqlite3_free_table(tab->result);
	__free_function((void **)&tab->cmd);
	__free_function((void **)&tab);
	__free_function((void **)&buf);
	return 0;
}

int The_server_obtains_the_administrator_information_table(PMSG pmsg, int *newfd)//服务端获取管理员信息表
{
	int ret = 0, i, j;
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(MB);
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	strcpy(tab->cmd, "select * from admin_table");
	db = __sqlite3_open_func_(TAB_NAME);
	
	__sqlite3_get_table_func_(db, tab);
	
	for(i = 1; i <= tab->row; i++)
	{
			
		for(j = 0; j < tab->column; j++)
		{
			if(ret >= MB-20)//xiaopan
			{
				memset(pmsg->data, 0, sizeof(pmsg->data));
				strcpy(pmsg->data, buf);
				The_server_sends_information_to_the_client(pmsg, newfd);
				memset(buf, 0, sizeof(MB));
				ret = 0;
			}
			ret = sprintf(buf+ret, "%16s ", tab->result[i*tab->column+j]);
		}
		ret = sprintf(buf+ret, "\n");
			
	}
	printf("%s\n", buf);
	memset(pmsg->data, 0, sizeof(pmsg->data));
	strcpy(pmsg->data, buf);
	The_server_sends_information_to_the_client(pmsg, newfd);
	__sqlite3_close_func_(db);
	sqlite3_free_table(tab->result);
	__free_function((void **)&tab->cmd);
	__free_function((void **)&tab);
	__free_function((void **)&buf);
	return 0;
}


int The_server_resolves_the_user_account_information_requested_by_the_client(PMSG pmsg, int *newfd)//服务端解析客户端请求用户账号信息操作函数_
{
	int tabnum;
	The_server_receives_information_from_the_client(pmsg, newfd);

	tabnum = (pmsg->flags[0] >> 6) & 0x3;
	printf("%d\n", tabnum);
	if(tabnum == 1)
	{
		The_server_obtains_the_common_employee_information_table(pmsg, newfd);//服务端获取普通员工信息表_
	}
	else if(tabnum == 0)
	{
		The_server_obtains_the_administrator_information_table(pmsg, newfd);//服务端获取管理员信息表
	}

	
	return 0;
}

int The_server_resolves_the_client_and_modifies_the_user_account_information(PMSG pmsg, int *newfd)//服务端解析客户端修改用户账号信息
{
	int ret = 0;
//	The_server_resolves_the_user_account_information_requested_by_the_client(pmsg, newfd);//服务端解析客户端请求用户账号信息操作函数_
	The_server_receives_information_from_the_client(pmsg, newfd);
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(SIZE);
	db = __sqlite3_open_func_(TAB_NAME);
	memset(pmsg->data, 0, sizeof(pmsg->data));
	memset(buf, 0, SIZE);
	if(Verify_account_existence(pmsg, db) == 1)//验证账号存在 
	{
		strcpy(pmsg->data, "账号不存在!");
	}
	else
	{
		ret = sprintf(buf, "update staff_info_tab set passwd=\"%s\" where id=%d;",pmsg->passwd, atoi(pmsg->id));
		if(ret < 0)
		{
			printf("%s %d: error!\n", __func__, __LINE__);
		}
		__sqlite3_exec_function_(db, buf);
		memset(buf, 0, SIZE);
		ret = sprintf(buf, "update staff_info_tab set passwd=\"%s\" where id=%d;",pmsg->passwd, atoi(pmsg->id));
		if(ret < 0)
		{
			printf("%s %d: error!\n", __func__, __LINE__);
		}
		__sqlite3_exec_function_(db, buf);
		memset(buf, 0, SIZE);
		ret = sprintf(buf, "update staff_info_tab set name=\"%s\" where id=%d;",pmsg->name, atoi(pmsg->id));
		if(ret < 0)
		{
			printf("%s %d: error!\n", __func__, __LINE__);
		}
		__sqlite3_exec_function_(db, buf);
		memset(buf, 0, SIZE);
		ret = sprintf(buf, "update staff_info_tab set sex=%d where id=%d;",pmsg->sex, atoi(pmsg->id));
		if(ret < 0)
		{
			printf("%s %d: error!\n", __func__, __LINE__);
		}
		__sqlite3_exec_function_(db, buf);
		memset(buf, 0, SIZE);
		ret = sprintf(buf, "update staff_info_tab set wage=\"%s\" where id=%d;",pmsg->wage, atoi(pmsg->id));
		if(ret < 0)
		{
			printf("%s %d: error!\n", __func__, __LINE__);
		}
		__sqlite3_exec_function_(db, buf);
		memset(buf, 0, SIZE);
		ret = sprintf(buf, "update staff_info_tab set telephone=\"%s\" where id=%d;",pmsg->telephone, atoi(pmsg->id));
		if(ret < 0)
		{
			printf("%s %d: error!\n", __func__, __LINE__);
		}
		__sqlite3_exec_function_(db, buf);
		memset(buf, 0, SIZE);
		ret = sprintf(buf, "update staff_info_tab set addr=\"%s\" where id=%d;",pmsg->addr, atoi(pmsg->id));
		if(ret < 0)
		{
			printf("%s %d: error!\n", __func__, __LINE__);
		}
		__sqlite3_exec_function_(db, buf);
		memset(buf, 0, SIZE);
	}	
	__sqlite3_close_func_(db);
	__free_function((void **)&buf);
	The_server_sends_information_to_the_client(pmsg, newfd);
	return 0;
}

int Verify_account_existence(PMSG pmsg, sqlite3 *db)//验证账号存在 
{
	int ret = 1, i = 0, flags = 0;
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	ret = sprintf(tab->cmd, "select id from staff_info_tab;");
	if(ret < 0)
	{
		printf("%s %d: error!\n", __func__, __LINE__);
	}
	__sqlite3_get_table_func_(db, tab);
	for(i = 1; i <= tab->row; i++)
	{
		if(strcmp(tab->result[i], pmsg->id) == 0)
		{
			flags = 0;
			break;
		}
	}
	sqlite3_free_table(tab->result);
	__free_function((void **)&tab);
	__free_function((void **)&tab);
	return flags;
}

int The_server_parses_the_client_delete_account_function(PMSG pmsg, int *newfd)//服务端解析客户端删除账户函数
{
	int ret = 0;
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(SIZE);
	The_server_receives_information_from_the_client(pmsg, newfd);
	db = __sqlite3_open_func_(TAB_NAME);
	ret = sprintf(buf, "delete from staff_info_tab where id=%d;", atoi(pmsg->id));
	if(ret < 0)
	{
		printf("%s %d: sprintf!\n", __func__, __LINE__);
		strcpy(pmsg->data, "error!");
	}
	__sqlite3_exec_function_(db, buf);
	__sqlite3_close_func_(db);
	__free_function((void **)&buf);
	The_server_sends_information_to_the_client(pmsg, newfd);
	return 0;
}

int The_server_parses_the_client_operation_functions(PMSG pmsg, int *newfd)//服务器解析客户端操作函数
{
	int ret = 0;
	sqlite3 *db = NULL;
	int statas = 0;
	char *buf = (char *)__malloc_function_(SIZE);
	The_server_receives_information_from_the_client(pmsg, newfd);
	db = __sqlite3_open_func_(TAB_NAME);
	ret = sprintf(buf,\
		       	"create table if not exists staff_info_tab (id int primary key, \
passwd char, name char, sex char, addr char, wage float, telephone long, statas int);");
	if(ret < 0)
	{
		printf("%s %d :sprintf!\n", __func__, __LINE__);
	}
	__sqlite3_exec_function_(db, buf);
	memset(buf, 0, SIZE);
	statas |= 0x1 << 4;
	ret = sprintf(buf,\
"insert into staff_info_tab (id, passwd, name, wage, statas) values (%d, \"%s\", \"%s\", %.2f, %d);", \
 atoi(pmsg->id), pmsg->name, pmsg->name, atof(pmsg->wage), statas);
	if(__sqlite3_exec_function_(db, buf) == 19)
	{
		strcpy(pmsg->data, "账户已经存在!");
	}
	else
	{
		strcpy(pmsg->data, "账户开通成功!");
	}
	memset(buf, 0, SIZE);
	The_server_sends_information_to_the_client(pmsg, newfd);
	__sqlite3_close_func_(db);
	__free_function((void **)&buf);
	return 0;
}

int Account_password_Login_information_authentication_function(PMSG pmsg, int *newfd)//账号密码登录信息验证函数
{
	int i;
	int flags = 0;
	PTAB tab = (PTAB)__malloc_function_(sizeof(TAB));
	tab->cmd = (char *)__malloc_function_(SIZE);
	strcpy(tab->cmd, "select id, passwd, stats from admin_table");
	sqlite3 *db = __sqlite3_open_func_(TAB_NAME);
	__sqlite3_get_table_func_(db, tab);
	for(i = 1;i < tab->row+1; i++)
	{
		if(atoi(tab->result[tab->column*i]) == atoi(pmsg->id))
		{
			flags = 1;
			break;
		}
	}
	if(flags == 1)
	{
		for(i = 1;i < tab->row+1; i++)
		{
			if(strncasecmp(tab->result[tab->column*i+1], pmsg->passwd, strlen(pmsg->passwd)) == 0)
			{
				if(strcmp(tab->result[tab->column*i+2], "1") == 0)
					flags = 3;	
				else
					flags = 2;
				break;
			}
		}
	}
	Result_of_determining_login_information(pmsg, newfd, flags);
	sqlite3_free_table(tab->result);
	__sqlite3_close_func_(db);
	__free_function((void **)&tab->cmd);
	__free_function((void **)&tab);
	return flags;
}


int Result_of_determining_login_information(PMSG pmsg, int *newfd, int flags)//登录信息判定结果_
{
	memset(pmsg->data, 0, sizeof(pmsg->data));
	switch(flags)
	{
		case 0:
			pmsg->flags[0] &= ~(0x3 << 4);
			pmsg->flags[0] |= flags << 4;
			strcpy(pmsg->data, "用户名不村在!");
			break;
		case 1:
			pmsg->flags[0] &= ~(0x3 << 4);
			pmsg->flags[0] |= flags << 4;
			strcpy(pmsg->data, "密码输入错误!");
			break;
		case 2:
			pmsg->flags[0] &= ~(0x3 << 4);
			pmsg->flags[0] |= flags << 4;
			strcpy(pmsg->data, "登录成功!");
			Records_the_login_status_bits(pmsg, newfd, 1);//记录登录状态位_
			break;	
		case 3:
			pmsg->flags[0] &= (0x3 << 4);
			pmsg->flags[0] |= flags << 4;
			strcpy(pmsg->data, "该账号已经登录!");
			break;
	}
	The_server_sends_information_to_the_client(pmsg, newfd);//这个发送个服务器
	return 0;
}

int general_staff_Records_the_login_status_bits(PMSG pmsg, int *newfd, int flags)//普通员工记录登录状态位_
{
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(SIZE);
	db = __sqlite3_open_func_(TAB_NAME);
	sprintf(buf, "update staff_info_tab set statas=%d where id=%s;", flags, pmsg->id);
	__sqlite3_exec_function_(db, buf);
	__sqlite3_close_func_(db);
	__free_function((void **)&buf);
	return 0;
}


int Records_the_login_status_bits(PMSG pmsg, int *newfd, int flags)//记录登录状态位_
{
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(SIZE);
	db = __sqlite3_open_func_(TAB_NAME);
	sprintf(buf, "update admin_table set stats=%d where id=\"%s\";", flags, pmsg->id);
	__sqlite3_exec_function_(db, buf);
	__sqlite3_close_func_(db);
	__free_function((void **)&buf);
	return 0;
}

int __sqlite3_get_table_func_(sqlite3 *db, PTAB tab)
{	
	int ret = 0;
	ret = sqlite3_get_table(db, tab->cmd, &tab->result, &tab->row, &tab->column, &tab->err);
	if(ret != 0)
	{
		printf("%s %d: %s\n", __func__, __LINE__, tab->err);
	}
	return ret;
}


sqlite3 *__sqlite3_open_func_(char *path)
{
	sqlite3 *db = NULL;
	int ret = 0;
	ret = sqlite3_open(path, &db);
	if(ret != SQLITE_OK)
	{
		printf("%s %d: sqlite3 open is error!\n", __func__, __LINE__);
		return NULL;
	}
	return db;
}

int __sqlite3_close_func_(sqlite3 *db)
{
	int ret = 0;
	ret = sqlite3_close(db);
	if(ret != 0)
	{
		printf("%s %d: sqlite3 close is error!\n", __func__, __LINE__);
		return ret;
	}
	return 0;	
}

int __sqlite3_exec_function_(sqlite3* db, char *sql_cmd)
{
	int ret = 0;
	char *errmsg = NULL;
	ret = sqlite3_exec(db, sql_cmd, NULL, NULL, &errmsg);
	if(ret != 0)
	{
		if(ret == 19)
		{
			printf("账号ID重复\n");
			return ret;
		}
		else
		{
			printf("%s %d: %s!\n", __func__, __LINE__, errmsg);
			sqlite3_free((void  *)errmsg);
			printf("=======%d======\n", ret);
			return ret;
		}
	}
	return 0;
}

int Server_administrator_registration_information_manipulation_function(PMSG pmsg, int *newfd)//服务器管理员注册信息操作函数
{
	sqlite3 *db = NULL;
	char *buf = (char *)__malloc_function_(SIZE);
	int ret = 0;
	The_server_receives_information_from_the_client(pmsg, newfd);
	ret = sqlite3_open(TAB_NAME, &db);
	if(ret != SQLITE_OK)
	{
		printf("%s %d: sqlite3 open is error!\n", __func__, __LINE__);
		return ret;
	}
	ret = sprintf(buf, "create table if not exists admin_table(id int primary key, passwd char, name char, stats int);");
	if(ret < 0 || ret > SIZE)
	{
		printf("%s %d: sprintf is error!\n", __func__, __LINE__);
		return -1;
	}
	__sqlite3_exec_function_(db, buf);
	memset(buf, 0, SIZE);
	ret = sprintf(buf, "insert into admin_table (id, passwd, name, stats) values (%d, \"%s\", \"%s\", %d);",\
		       	atoi(pmsg->id), pmsg->passwd, pmsg->name, 0);//0表示未登录 1表示登录
	if(ret < 0 || ret > SIZE)
	{
		printf("%s %d: sprintf is error!\n", __func__, __LINE__);
		return -1;
	}
	memset(pmsg->data, 0, sizeof(pmsg->data));
	if(__sqlite3_exec_function_(db, buf) == -1)
	{
		strcpy(pmsg->data, "账户已经存在!");
	}
	else
	{
		strcpy(pmsg->data, "账户注册成功!");
	}
	
	ret = sqlite3_close(db);
	if(ret != 0)
	{
		printf("%s %d: sqlite3 close is error!\n", __func__, __LINE__);
		return ret;
	}
	The_server_sends_information_to_the_client(pmsg, newfd);
	__free_function((void **)&buf);
	return 0;
}


int The_server_receives_information_from_the_client(PMSG pmsg, int *newfd)//服务端接收客户端信息_
{
	ssize_t ret = recv(*newfd, (void *)pmsg, sizeof(MSG), 0);
	if(ret < 0)
	{
		printf("%s %d: recv is error!\n", __func__, __LINE__);
		return -1;
	}
	else if(ret == 0)
	{
		Records_the_login_status_bits(pmsg, newfd, 0);
		general_staff_Records_the_login_status_bits(pmsg, newfd, 0);//普通员工记录登录状态位_
	}
	return 0;
}

int The_server_sends_information_to_the_client(PMSG pmsg, int *newfd)//服务端发送给客户端信息_
{
	ssize_t ret = send(*newfd, (void *)pmsg, sizeof(MSG), 0);
	if(ret < 0)
	{
		printf("%s %d: send is error!\n", __func__, __LINE__);
		return -1;
	}
	return 0;
}

void signal_handler(int signum)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
	printf("child is quit!\n");
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

void *__free_function(void **p)
{
	free(*p);
	*p = NULL;
	return NULL;
}
