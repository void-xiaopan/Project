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
#include <signal.h>
#include <sys/wait.h>
#include <sqlite3.h>


#define PORT 8888
#define IP "192.168.1.250"
#define TAB_NAME "./staff.db"
#define SIZE 256
#define MB 1024

typedef struct message
{
	char flags[8];
	char name[16];
	char id[12];//unsigned int
	char passwd[12];
	char sex;
	char addr[128];
	char wage[12];//float
	char telephone[12];//unsigned long
	char data[1024];
}__attribute__((packed))MSG, *PMSG;

typedef struct sqliet3_get_table
{
	char *cmd;
	int row;
	int column;
	char **result;
	char *err;
}TAB, *PTAB;




typedef void (*sighandler_t)(int);
void signal_handler(int signum);
void *__malloc_function_(size_t size);//申请指针内存
void *__free_function(void **p);//释放指针内存
int The_server_parses_client_operations(PMSG pmsg, int *newfd);//服务器解析客户端操作 
int Service_user_authentication_function(PMSG pmsg, int *newfd);//服务用户身份验证函数_
int Server_administrator_operation_resolution(PMSG pmsg, int *newfd);//服务器管理员操作解析_
int __sqlite3_exec_function_(sqlite3* db, char *sql_cmd);
int Server_administrator_registration_information_manipulation_function(PMSG pmsg, int *newfd);//服务器管理员注册信息操作函数
int The_server_receives_information_from_the_client(PMSG pmsg, int *newfd);//服务端接收客户端信息_
int The_server_sends_information_to_the_client(PMSG pmsg, int *newfd);//服务端发送给客户端信息_
int Server_administrator_login_information_operation_function(PMSG pmsg, int *newfd);//服务器管理员登录信息操作函数


int Account_password_Login_information_authentication_function(PMSG pmsg, int *newfd);//账号密码登录信息验证函数
sqlite3 *__sqlite3_open_func_(char *path);
int __sqlite3_close_func_(sqlite3 *db);
int __sqlite3_get_table_func_(sqlite3 *db, PTAB tab);
int Result_of_determining_login_information(PMSG pmsg, int *newfd, int flags);//登录信息判定结果_
int Records_the_login_status_bits(PMSG pmsg, int *newfd, int flags);//记录登录状态位_
int The_administrator_successfully_logs_in_to_the_function(PMSG pmsg, int *newfd);//管理员登录成功操作函数
int The_server_parses_the_client_operation_functions(PMSG pmsg, int *newfd);//服务器解析客户端操作函数
int The_server_parses_the_client_delete_account_function(PMSG pmsg, int *newfd);//服务端解析客户端删除账户函数
int The_server_resolves_the_user_account_information_requested_by_the_client(PMSG, int *);//服务端解析客户端请求用户账号信息操作函数_

int The_server_resolves_the_client_and_modifies_the_user_account_information(PMSG pmsg, int *newfd);//服务端解析客户端修改用户账号信息
int The_server_obtains_the_administrator_information_table(PMSG pmsg, int *newfd);//服务端获取管理员信息表
int The_server_obtains_the_common_employee_information_table(PMSG pmsg, int *newfd);//服务端获取普通员工信息表_
int Verify_account_existence(PMSG pmsg, sqlite3 *db);//验证账号存在 
int The_server_resolves_the_administrator_to_change_the_account_password(PMSG pmsg, int *newfd);//服务端解析管理员用户修改账户密码
int Server_resolution_The_administrator_queries_information_about_a_single_employee(PMSG pmsg, int *newfd);//服务器解析管理员查询单个员工信息_
int Server_administrator_view_all_employee_information(PMSG pmsg, int *newfd);//服务器管理员查看所有职工信息
int The_server_resolves_common_employee_operations(PMSG pmsg, int *newfd);//服务器解析普通职员操作
int The_server_resolves_the_common_user_login_operation_function(PMSG pmsg, int *newfd);//服务端解析普通用户登录操作函数_
int A_normal_user_validates_the_login_function(PMSG pmsg, int *newfd);//普通用户验证登录函数
int Verify_whether_this_is_the_user_first_login(PMSG pmsg, int *newfd);//验证是否是用户首次登录
int The_server_improves_the_user_information_function(PMSG pmsg, int *newfd);//服务器完善用户信息函数_
int The_server_parses_the_user_login_operation_function(PMSG pmsg, int *newfd);//服务端解析用户登录操作函数
int general_staff_Records_the_login_status_bits(PMSG pmsg, int *newfd, int flags);//普通员工记录登录状态位_
int The_server_parses_the_modification_information_of_common_employees(PMSG pmsg, int *newfd);//服务端解析普通员工修改信息
int The_server_parses_the_client_to_query_its_own_account_information(PMSG pmsg, int *newfd);//服务端解析客户查询自己账号信息
int The_server_resolves_the_client_query_address_book(PMSG pmsg, int *newfd);//服务端解析客户端查询通讯录



#endif
