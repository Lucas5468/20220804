#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "staff.h"

int do_admin(int sockfd, MSG *msg){

	msg->cmdtype = L;
//	msg->usertype = 1;

	printf("Input name: ");
	scanf("%s", msg->st.name);
	getchar();

	printf("Input pass: ");
	scanf("%s", msg->st.pass);
	
	if (send(sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send\n");
		return -1;
	}

	if (recv(sockfd, msg, sizeof(MSG), 0) < 0){
		perror("fail to recv");
		return -1;
	}

	if (strncmp(msg->data, "OK", 3) == 0){
		//添加管理员判断
		if (msg->st.usertype == 1){
			printf("Dear Adminer Login ok!\n");
			return 1;
		}
		else{
			printf("You is not Adminer!!!\n");
			return 0;
		}
	
	}else{
		printf("%s\n", msg->data);
	}

	return 0;

	return 0;
}

int do_user_login(int sockfd, MSG *msg){

	msg->cmdtype = L;
//	msg->usertype = 0;

	printf("Input name: ");
	scanf("%s", msg->st.name);
	getchar();

	printf("Input pass: ");
	scanf("%s", msg->st.pass);
	
	if (send(sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send\n");
		return -1;
	}

	if (recv(sockfd, msg, sizeof(MSG), 0) < 0){
		perror("fail to recv");
		return -1;
	}

	if (strncmp(msg->data, "OK", 3) == 0){
		//添加管理员判断
		printf("Login ok!\n");
		return 1;
	}else{
		printf("%s\n", msg->data);
	}

	return 0;
}

int do_query(int sockfd, MSG *msg){

	char *usrinfo[] = {"number", "usertype", "name", "pass", 
					   "age", "phone", "addr", "work", "date",
                       "level", "salary"};
	int i = 0;

	msg->cmdtype = Q;

	if (send(sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send\n");
		return -1;
	}

	if (recv(sockfd, msg, sizeof(MSG), 0) < 0){
		perror("fail to recv");
		return -1;
	}

	if (strncmp(msg->data, "OK", 3) == 0){
		//添加管理员判断
		printf("query ok!\n");
		
		//打印台头

		for (i = 0; i < 11; i++)
			printf("%s\t", usrinfo[i]);
		puts("");
		printf("---------------------------------------------\n");
		//打印查询的结果...
		printf("%d\t", msg->st.number);
		printf("%-8d\t", msg->st.usertype);
		printf("%s\t", msg->st.name);
		printf("%s\t", msg->st.pass);
		printf("%d\t", msg->st.age);
		printf("%s\t", msg->st.phone);
		printf("%s\t", msg->st.addr);
		printf("%s\t", msg->st.work);
		printf("%s\t", msg->st.date);
		printf("%d\t", msg->st.level);
		printf("%0.2f\n", msg->st.salary);
		return 1;
	}else{
		printf("%s\n", msg->data);
	}

	return 0;
}

int do_change(int sockfd, MSG *msg){

	msg->cmdtype = C;
	int n;

	printf("please Input Number (You Konwn!) :");
	scanf("%d", &msg->st.number);
	getchar();
	printf("please Input the option You want to change\n");
	printf("***1. addr  2. phone   3.passwd  4. quit**\n");
	printf("******************************************\n");
	printf("please Input your option :");
	scanf("%d", &n);
	getchar();

	switch(n){
		case 1:
			printf("please Input new addr: ");
			scanf("%s", msg->st.addr);
			strcpy(msg->data, "1");
			break;
		case 2:
			printf("please Input new phone: ");
			scanf("%s", msg->st.phone);
			strcpy(msg->data, "2");
			break;
		case 3:
			printf("please Input new passwd: ");
			scanf("%s", msg->st.pass);
			strcpy(msg->data, "3");
			break;
		case 4:
			return 0;
		default:
			printf("Input Invaild msg\n");
	}


	if (send(sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send\n");
		return -1;
	}

	if (recv(sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to recv\n");
		return -1;
	}

	if (strncmp(msg->data, "OK", 3) == 0){
		printf("change database success!\n");
		printf("change end!\n");
		return 1;
	}else{
		printf("%s\n", msg->data);
	}

	return 0;
}

int do_admin_query(int sockfd, MSG *msg){
	
	char *usrinfo[] = {"number", "usertype", "name", "pass", 
					   "age", "phone", "addr", "work", "date",
                       "level", "salary"};
	
	int i,n = 0;

	msg->cmdtype = Q;
//	msg->usertype = 1;

	printf("*************************************************\n");
	printf("**1.按人名查找   2.查找所有     3.退出 **********\n");
	printf("*************************************************\n");
	printf("请输入您的选择 （数字）: ");
	scanf("%d", &n);
	getchar();

	switch (n) {
		case 1:
			printf("请输入您要查找的用户名 :");
			scanf("%s", msg->st.name);
			break;
		case 2:
			msg->cmdtype = QA;
			send(sockfd, msg, sizeof(MSG), 0);

			/*打印台头*/
			for (i = 0; i < 11; i++)
				printf("%s\t", usrinfo[i]);
			puts("");
			printf("---------------------------------------------\n");

			/*循环接受服务器，传递过来的每条信息*/
			while(1){
				
				if (recv(sockfd, msg, sizeof(MSG), 0) < 0){
					printf("fail to recv");
				}

				if ('\0' == msg->st.name[0])
					break;

				printf("%d\t", msg->st.number);
				printf("%-8d\t", msg->st.usertype);
				printf("%s\t", msg->st.name);
				printf("%s\t", msg->st.pass);
				printf("%d\t", msg->st.age);
				printf("%s\t", msg->st.phone);
				printf("%s\t", msg->st.addr);
				printf("%s\t", msg->st.work);
				printf("%s\t", msg->st.date);
				printf("%d\t", msg->st.level);
				printf("%0.2f\n", msg->st.salary);

		//		sleep(1);
			}

			return 0;
		case 3:
			return 0;

	}

	if ( send(sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send\n");
		return -1;
	}

	if ( recv(sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to recv\n");
		return -1;
	}

	if (strncmp(msg->data, "OK", 3) == 0){
		//添加管理员判断
		printf("query ok!\n");
		
		//打印台头

		for (i = 0; i < 11; i++)
			printf("%s\t", usrinfo[i]);
		puts("");
		printf("---------------------------------------------\n");
		//打印查询的结果...
		printf("%d\t", msg->st.number);
		printf("%d\t", msg->st.usertype);
		printf("%s\t", msg->st.name);
		printf("%s\t", msg->st.pass);
		printf("%d\t", msg->st.age);
		printf("%s\t", msg->st.phone);
		printf("%s\t", msg->st.addr);
		printf("%s\t", msg->st.work);
		printf("%s\t", msg->st.date);
		printf("%d\t", msg->st.level);
		printf("%0.2f\n", msg->st.salary);
		return 1;
	}else{
		printf("%s\n", msg->data);
	}


	return 0;
}

int do_add_user(int sockfd, MSG *msg){

	msg->cmdtype = AU;
	
	/*添加新用户之前，把自己的用户名保存下来*/
//	printf("my name is %s\n",msg->st.name);
//	strcpy(msg->data, msg->st.name);

	printf("************Welcome New Employees************\n");
	printf("please Input staffno : ");
	scanf("%d", &msg->st.number);
	getchar();
	printf("The staffno is : %d\n", msg->st.number);
/*	printf("The staffno Once confirmed it cannot be changed (Y/N)");
	ch = getchar();
*/	
	printf("please Input name : ");
	scanf("%s", msg->st.name);
	getchar();

	printf("please Input passwd : ");
	scanf("%s", msg->st.pass);
	getchar();

	printf("please Input age : ");
	scanf("%d", &msg->st.age);
	getchar();

	printf("please Input phone : ");
	scanf("%s", msg->st.phone);
	getchar();
	
	printf("please Input addr : ");
	scanf("%s", msg->st.addr);
	getchar();

	printf("please Input work : ");
	scanf("%s", msg->st.work);
	getchar();

	printf("please Input date : ");
	scanf("%s", msg->st.date);
	getchar();

	printf("please Input level : ");
	scanf("%d", &msg->st.level);
	getchar();

	printf("please Input salary : ");
	scanf("%f", &msg->st.salary);
	getchar();

	printf("IS NOT adminer : (Y/N)");
	char ch[1]; 
	ch[0] = getchar();
	if (0 == strncasecmp(ch, "Y", 1)) {
		msg->st.usertype = 1;
	//	printf("yyy\n");
	}else{
		msg->st.usertype = 0;
	//	printf("nnn\n");
	}
	getchar();


	if (send (sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send\n");
	}

	if (recv(sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to recv\n");
	}

 
	if ( 0 == strncmp(msg->data, "OK", 3)){
		printf("Add success!!!\n");
	}else {
		printf("Add failed...\n");
		return 0;
	}

	return 0;
}

int do_del_user(int sockfd, MSG *msg){

	msg->cmdtype = DU;

//	strcpy(msg->data, msg->st.name);
	
	printf("---------------------------------------------\n");
	printf("please Input staffno You want to delete :");
	scanf("%d", &msg->st.number);
	getchar();

	printf("please Input name You want to delete : ");
	scanf("%s", msg->st.name);
	getchar();

	if (send (sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send\n");
	}

	if (recv(sockfd, msg, sizeof(MSG), 0) < 0){
		printf("fail to recv\n");
	}

 
	if ( 0 == strncmp(msg->data, "OK", 3)){
		printf("Del success!!!\n");
	}else {
		printf("Del failed...\n");
		return 0;
	}

	
	return 0;
}

int do_history(int sockfd, MSG *msg){

	msg->cmdtype = H;

	send(sockfd, msg, sizeof(MSG), 0);

	/*打印台头*/

	printf("   TIME              USER        WORDS \n");
	    //    2022-07-29 17:54:56---4       ---user 4 is Added!
	while(1){
		
		recv(sockfd, msg, sizeof(MSG), 0);

		if ('\0' == msg->data[0])
			break;

		printf("%s\n", msg->data);
	}

	return 0;
}


int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in sin;
	
	MSG msg;

	//参数检查
	if (argc != 3){
		printf("Usage: %s serverip port.\n", argv[0]);
		return -1;
	}

	/*1.创建套接字*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		perror("fail to connect");
		return -1;
	}

	/*2.1 填充结构体*/
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	sin.sin_port = htons(atoi(argv[2]));

	/*2.2 连接服务器*/
	if ( connect(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("fail to connect");
		return -1;
	}

	int n;
	char buf[BUFSIZ];

	while(1){
		printf("*************************************************\n");
		printf("**1.管理员模式  2.普通用户模式  3.退出***********\n");
		printf("*************************************************\n");
restart:
		printf("please choose(1|2|3): ");

		scanf("%s", buf);
		n = atoi(buf);
		if (strlen(buf) != 1){
			printf("Input cmd Invaild\n");
			goto restart;
		}

		if (n != 1 && n != 2 && n != 3 ){
			printf("Input cmd Invaild\n");
			goto restart;
		}

		switch (n) {
			case 1:
				if (do_admin(sockfd, &msg) == 1){
					goto admin_next;
				}
				break;
			case 2:
				if (do_user_login(sockfd, &msg) == 1){
					goto user_next;
				}
				break;
			case 3:
				close(sockfd);
				exit(0);
				break;
			default:
				printf("Input cmd Invaild\n");				
		}
	}

admin_next:
		while(1){
			printf("*************************************************\n");
			printf("**1.查询        2.修改          3.添加用户*******\n");
			printf("**4.删除用户    5.查询历史记录  6.退出***********\n");
			printf("*************************************************\n");
			printf("please choose:");
			scanf("%d", &n);
			getchar();

			switch (n){
				case 1:
					do_admin_query(sockfd, &msg);
					break;
				case 2:
					do_change(sockfd, &msg);
					break;
				case 3:
					do_add_user(sockfd,&msg);
					break;
				case 4:
					do_del_user(sockfd,&msg);
					break;
				case 5:
					do_history(sockfd,&msg);
					break;
				case 6:
					close(sockfd);
					exit(0);
					break;
				default:
					printf("Invaild data cmd.\n");
			}
			
		}


user_next:
		while(1){
			printf("*************************************************\n");
			printf("**1.查询        2.修改          3.退出***********\n");
			printf("*************************************************\n");
			printf("please choose:");
			scanf("%d", &n);
			getchar();

			switch (n){
				case 1:
					do_query(sockfd, &msg);
					break;
				case 2:
					do_change(sockfd, &msg);
					break;
				case 3:
					close(sockfd);
					exit(0);
					break;
				default:
					printf("Invaild data cmd.\n");
			}
		}

	return 0;
}
