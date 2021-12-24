#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	//打开数据库
	sqlite3* db = NULL;
	if(sqlite3_open("./dict.db", &db) != SQLITE_OK)
	{
		printf("ERROR:%s __%d__\n", sqlite3_errmsg(db), __LINE__);
		return -1;
	}

	//创建一张表格(word, mean)
	char sql[500] = "create table if not exists dict (word char, mean char);";
	char* errmsg = NULL;
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("ERROR:%s __%d__\n", errmsg, __LINE__);
		return -1;
	}
	printf("create table success\n");
	
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
		sprintf(sql, "insert into dict values(\"%s\", \"%s\")", word, mean);
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
	//关闭数据库
	sqlite3_close(db);
	return 0;
}

