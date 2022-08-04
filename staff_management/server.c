#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>
#include "staff.h"

char name[16] = {0};  //保存用户名

int get_time(char *time_data);

int do_admin(int acceptfd, MSG *msg, sqlite3 *db){
	return 0;
}

int do_user_login(int acceptfd, MSG *msg, sqlite3 *db){
	char sql[128];
	char *errmsg;
	int nrow, ncloumn;
	char **resultp;

	strcpy(name, msg->st.name);

	sprintf(sql, "select * from usrinfo where name='%s' and passwd='%s';", msg->st.name, msg->st.pass);

	/*查询函数*/
	if (sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
		return -1;
	}else {
		printf("get_table ok!!!\n");
	}

	/*判断查询结果是否唯一*/
	if (nrow == 1 ){ //说明查询到一行数据，成功
		strcpy(msg->data, "OK");
		//root
		msg->st.usertype = atoi(resultp[ncloumn+1]);
		send(acceptfd, msg, sizeof(MSG), 0);
		return 1;
	}else {
		strcpy(msg->data, "usr/passwd wrong."); 
		send(acceptfd, msg, sizeof(MSG), 0);
	}

	return 0;
}

int do_query(int acceptfd, MSG *msg, sqlite3 *db){
	char sql[128];
	char *errmsg;
	int nrow, ncloumn;
	char **resultp;

//	if ( 0 == msg->usertype)
//		sprintf(sql, "select * from usrinfo where name='%s' and passwd='%s';", msg->st.name, msg->st.pass);
//	else
		sprintf(sql, "select * from usrinfo where name='%s';", msg->st.name);

	/*查询函数*/
	if (sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
		return -1;
	}else {
		printf("get_table ok!!!\n");
	}

	/*判断查询结果是否唯一*/
	if (nrow == 1 ){ //说明查询到一行数据，成功
		strcpy(msg->data, "OK");
		//root
		memset(&msg->st, 0, sizeof(struct info));
		//填充查询的结构体信息
		msg->st.number = atoi(resultp[ncloumn]);
		msg->st.usertype = atoi(resultp[ncloumn+1]);
		strncpy(msg->st.name, resultp[ncloumn+2], strlen(resultp[ncloumn+2]));
		strncpy(msg->st.pass, resultp[ncloumn+3], strlen(resultp[ncloumn+3]));
		msg->st.age = atoi(resultp[ncloumn+4]);
		strncpy(msg->st.phone, resultp[ncloumn+5], strlen(resultp[ncloumn+5]));
		strncpy(msg->st.addr, resultp[ncloumn+6], strlen(resultp[ncloumn+6]));
		strncpy(msg->st.work, resultp[ncloumn+7], strlen(resultp[ncloumn+7]));
		strncpy(msg->st.date, resultp[ncloumn+8], strlen(resultp[ncloumn+8]));
		msg->st.level = atoi(resultp[ncloumn+9]);
		msg->st.salary = atoi(resultp[ncloumn+10]);
		
		send(acceptfd, msg, sizeof(MSG), 0);
		return 1;
	}else {
		strcpy(msg->data, "query wrong.");
		send(acceptfd, msg, sizeof(MSG), 0);
	}
	return 0;
}

int allusr_callback(void *arg, int f_num, char **f_value, char ** f_name){
	
	int acceptfd;
	MSG msg;

	acceptfd = *(int *)arg;

	memset(&msg.st, 0, sizeof(struct info));
	msg.st.number = atoi(f_value[0]);
	msg.st.usertype = atoi(f_value[1]);
	strncpy(msg.st.name, f_value[2], strlen(f_value[2]));
	strncpy(msg.st.pass, f_value[3], strlen(f_value[3]));
	msg.st.age = atoi(f_value[4]);
	strncpy(msg.st.phone, f_value[5], strlen(f_value[5]));
	strncpy(msg.st.addr, f_value[6], strlen(f_value[6]));
	strncpy(msg.st.work, f_value[7], strlen(f_value[7]));
	strncpy(msg.st.date, f_value[8], strlen(f_value[8]));
	msg.st.level = atoi(f_value[9]);
	msg.st.salary = atoi(f_value[10]);

//	printf("send before\n");
	send(acceptfd, &msg, sizeof(MSG), 0);

//	printf("callback endding\n");
//	printf("f_num=%d\n", f_num);

	return 0;

}

int do_admin_query(int acceptfd, MSG *msg, sqlite3 *db){
	char sql[128];
	char *errmsg;

	sprintf(sql, "select * from usrinfo;");

	/*查询数据库*/
	if (sqlite3_exec(db, sql, allusr_callback, (void *)&acceptfd, &errmsg) != SQLITE_OK) {
		printf("%s\n", errmsg);
	}else{
		printf("callback ending!!!\n");
	}
	
//	printf("111\n");

	/*最后发送一次 '\0'*/
	msg->st.name[0] = '\0';

//	printf("send before\n");

	send(acceptfd, msg, sizeof(MSG), 0);

//	printf("send after\n");

	return 0;
}

int do_change(int acceptfd, MSG *msg, sqlite3 *db){
	
	char sql[512] = {0};
	char sql1[600] = {0};
	char *errmsg;
	char time_data[16] = {0};
	char words[512] = {0};
	
	char recv_tpye = msg->data[0];

	switch(recv_tpye){
		case '1':
			sprintf(sql, "update usrinfo set addr='%s' where staffno=%d;", msg->st.addr, msg->st.number);
			break;
		case '2':
			sprintf(sql, "update usrinfo set phone='%s' where staffno=%d;", msg->st.phone, msg->st.number);
			break;
		case '3':
			sprintf(sql, "update usrinfo set passwd='%s' where staffno=%d;", msg->st.pass, msg->st.number);
			break;
		default:
			printf("recv_tpye Invaild\n");
	}

	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
		printf("%s\n",errmsg);
		strcpy(msg->data, "change failed.");
	}else{
		printf("update success\n");

		get_time(time_data);

		switch (recv_tpye){
			case '1':
				sprintf(words, "The Adminer %s changed The addr of staffno:%d to %s", name, msg->st.number, msg->st.addr);
				break;
			case  '2':
				sprintf(words, "The Adminer %s changed The phone of staffno:%d to %s", name, msg->st.number, msg->st.phone);
				break;
			case '3':
				sprintf(words, "The Adminer %s changed The passwd of staffno:%d to %s", name, msg->st.number, msg->st.pass);
				break;
			default:
				printf("recv_tpye Invaild\n");
		}

		sprintf(sql1, "insert into historyinfo values('%s','%s','%s');", time_data, name, words);
//		printf("%s\n", sql1);

		if (sqlite3_exec(db, sql1, NULL, NULL, &errmsg) != SQLITE_OK){
			printf("%s\n", errmsg);
			return -1;
		}

		strcpy(msg->data, "OK");
	}

	if (send(acceptfd, msg, sizeof(MSG), 0) < 0){
		perror("fail to send");
		return -1;
	} 

	return 0;
}

int do_add_user(int acceptfd, MSG *msg, sqlite3 *db){
	char sql[500];
	char sql1[500];
	char *errmsg;
	char time_data[16];
	char words[128];

/*	if ( recv(acceptfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to recv\n");
	}
*/	
//	printf("");
	
	sprintf(sql, "insert into usrinfo values(%d, %d, '%s', '%s', %d, '%s', '%s', '%s', '%s', %d, '%f');", msg->st.number, msg->st.usertype, msg->st.name,
									msg->st.pass, msg->st.age, msg->st.phone,
									msg->st.addr, msg->st.work,msg->st.date,
									msg->st.level,msg->st.salary);

	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
		strcpy(msg->data, "Add failed!");
	}else {
		printf("Add success!!\n");
	//	memset(sql, 0, sizeof(sql));
		get_time(time_data);
		sprintf(words, "The Adminer added a new user %s", msg->st.name);

		sprintf(sql1, "insert into historyinfo values('%s','%s','%s');", time_data, name, words);
//		printf("%s\n", sql1);

		if (sqlite3_exec(db, sql1, NULL, NULL, &errmsg) != SQLITE_OK){
			printf("%s\n", errmsg);
			return -1;
		}


		strcpy(msg->data,"OK");
	}

	if (send(acceptfd, msg, sizeof(MSG), 0) < 0){
		perror("fail to send");
		return -1;
	} 

	return 0;
}

int do_del_user(int acceptfd, MSG *msg, sqlite3 *db){
	char sql[128];
	char sql1[600];
	char *errmsg;
	char time_data[16];
	char words[128];

	sprintf(sql, "delete from usrinfo where staffno=%d and name='%s';", msg->st.number, msg->st.name);
	
	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
		strcpy(msg->data, "Del failed!");
	}else {
		printf("Del success!!\n");
		get_time(time_data);
		sprintf(words, "The Adminer delete a  user %s", msg->st.name);

		sprintf(sql1, "insert into historyinfo values('%s','%s','%s');", time_data, name, words);
//		printf("%s\n", sql1);

		if (sqlite3_exec(db, sql1, NULL, NULL, &errmsg) != SQLITE_OK){
			printf("%s\n", errmsg);
			return -1;
		}
		strcpy(msg->data,"OK");
	}

	if (send(acceptfd, msg, sizeof(MSG), 0) < 0){
		perror("fail to send");
		return -1;
	} 
	return 0;
}

int get_time(char *time_data){
	
	time_t t;
	struct tm *tp;
	time(&t);
	tp = localtime(&t);

	sprintf(time_data, "%4d-%02d-%2d %02d:%02d:%02d", tp->tm_year+1900, tp->tm_mon+1,
													  tp->tm_mday, tp->tm_hour,
													  tp->tm_min, tp->tm_sec);
	return 0;
}

int history_callback(void *arg, int f_num, char **f_value, char** f_name){
	/*time  name  words*/
	int acceptfd;
	MSG msg;

	acceptfd = *(int *)arg;

	sprintf(msg.data, "%s---%-8s---%s", f_value[0], f_value[1], f_value[2]);
	send(acceptfd, &msg, sizeof(MSG), 0);

	return 0;
}

int do_history(int acceptfd, MSG *msg, sqlite3 *db){

	char sql[128];
	char *errmsg;

	sprintf(sql, "select * from historyinfo");

	if (sqlite3_exec(db, sql, history_callback, (void *)&acceptfd, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
	}

	msg->data[0] = '\0';
	send(acceptfd, msg, sizeof(MSG), 0);

	return 0;
}
int do_client(int acceptfd, sqlite3 *db){
	MSG msg;

	while(recv(acceptfd, &msg, sizeof(msg), 0) > 0){
		printf("recv msg.cmdtype=%d\n", msg.cmdtype);
		switch (msg.cmdtype){
			case L:
				do_user_login(acceptfd, &msg, db);
				break;
			case Q:
				do_query(acceptfd, &msg, db);
				break;
			case C:
				do_change(acceptfd, &msg, db);
				break;
			case QA:
				do_admin_query(acceptfd,&msg, db);
				break;
			case AU:
				do_add_user(acceptfd, &msg, db);
				break;
			case DU:
				do_del_user(acceptfd, &msg, db);
				break;
			case H:
				do_history(acceptfd, &msg, db);
				break;
			default:
				printf("Invaild data msg\n");
		}
	}

	printf("Client quit\n");
	close(acceptfd);
	exit(0);

	return 0;
}

int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in sin;
	sqlite3 *db;

	//参数检查
	if (argc != 3){
		printf("Usage: %s serverip port.\n", argv[0]);
		return -1;
	}

	/* 打开数据库 */
	if (sqlite3_open(DATABASE, &db) != SQLITE_OK){
		printf("%s\n", sqlite3_errmsg(db));
		return -1;
	}else{
		printf("open DATABASE success\n");
	}

	/*打开数据库后新建 root管理员，否则已经存在*/
	// sqlite3_exec();


	/*1.创建套接字*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		perror("fail to connect");
		return -1;
	}
	
	/*1.1 设置套接字属性，可以重启*/
	int b_reuse = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &b_reuse, sizeof(int));

	/*2. 填充结构体, 并且bind*/
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	sin.sin_port = htons(atoi(argv[2]));
	
	if (bind(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("bind");
		return -1;
	}

	/*3.listen 主动变被动 */
	if (listen(sockfd, 5)){
		perror("listen");
		return -1;
	}

	int acceptfd;
	pid_t pid;

	/*信号的方式回收子进程
	 * 父进程忽略子进程发出的信号，让内核的init进程接管*/
	signal(SIGCHLD, SIG_IGN);

	while(1){
		if ((acceptfd = accept(sockfd, NULL, NULL)) < 0){
			perror("accept fail");
			return -1;
		}
		/*用多进程的方式实现并发*/
		if ((pid = fork()) < 0){
			perror("fork");
			return -1;
		}else if (pid == 0){

			close(sockfd);
			do_client(acceptfd,db);

		}else{
			close(acceptfd);
		}
	}
	return 0;
}
