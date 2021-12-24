#ifndef __STAFF_INCLUDE_H__
#define __STAFF_INCLUDE_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>


#define PORT 8888
#define IP "192.168.1.250"

typedef struct message_t
{
	char flags[8];
	char name[16];
	char id[12];
	char passwd[12];
	char sex;
	char addr[128];
	char wage[12];
	char telephone[12];
	char data[1024];
}__attribute__((packed))MSG, *PMSG;


int The_client_receives_the_message(PMSG pmsg);//客户端接收信息 
int The_client_sends_the_message(PMSG pmsg);//客户端发送消息
int user_connection_operation(PMSG pmgs);//用户连接操作 
int The_administrator_registers_the_account_function(PMSG pmsg);//管理员注册账号函数_
int Administrator_Operation_Tips();//管理员操作提示 

int get_user_id(PMSG pmsg);
int get_user_name(PMSG pmsg);
int get_user_sex(PMSG pmsg);
int get_user_passwd(PMSG pmsg);
int get_user_addr(PMSG pmsg);
int get_user_wage(PMSG pmsg);
int get_user_telephone(PMSG pmsg);
int get_user_data(PMSG pmsg);
int Administrator_operation_function(PMSG pmsg);//管理员操作函数

void *pthread_func(void *arg_v);
int user_identity_selection_prompt();//用户身份选择
void *__malloc_function_(size_t size);//申请指针内存
void *__free_function_(void **p);//释放指针内存

int Administrator_login_account_function(PMSG pmsg);//管理员登录账号函数
int Prompt_function_for_administrator_login_operation(PMSG pmsg);//管理员登录操作提示函数
int The_administrator_successfully_logs_in_to_the_function(PMSG pmsg);//管理员登录成功操作函数
int Add_employee_information_manipulation_functioni(PMSG pmsg);//增加员工信息操作函数
int Delete_employee_account_information_operation_function(PMSG pmsg);//删除员工账户信息操作函数
int Client_user_modifies_information_operation_function(PMSG pmsg);//客户端用户修改信息操作函数
int Funcddtion_used_by_client_users_to_obtain_user_information(PMSG pmsg);//客户端用户获取用户信息操作函数_
int Example_Modify_the_account_information_of_a_user(PMSG pmsg);//修改用户自己的账号信息
int The_administrator_queries_information_about_an_employee(PMSG pmsg);//管理员查询单个员工信息_
int The_administrator_queries_information_about_all_employees(PMSG pmsg);//管理员查询所有员工信息
int Client_common_users_operate_functions(PMSG pmsg);//客户端普通用户操作函数
int Common_user_operation_prompt_functioni();//_普通用户操作提示函数_
int Client_common_user_login_operation_function(PMSG pmsg);//_客户端普通用户登录操作函数
int The_client_user_successfully_logs_in_to_implement_the_function(PMSG pmsg);//客户端用户登录成功操作_实现函数_
int Normal_client_users_use_functions_normally(PMSG pmsg);//客户端普通用户正常使用函数
int A_message_indicating_successful_login_is_displayed(PMSG pmsg);//用户登录成功操作提示
int Common_client_users_modify_their_own_information(PMSG pmsg);//客户端普通用户修改自己的信息
int Modification_prompt_for_common_users();//普通用户修改提示_
int Ordinary_users_view_all_their_information(PMSG pmsg);//普通用户查看自己的所有信息
int Common_users_query_the_employee_address_book(PMSG pmsg);//普通用户查询员工通讯录





#endif
