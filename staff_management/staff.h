#ifndef _STAFF_H_
#define _STAFF_H_

#define L 1 // login
#define Q 2 // query--usr
#define C 3 // change

#define QA 4 // query--admin
#define AU 5 // do_add_user
#define DU 6 // do_del_user
#define H 7  // do_history

#define DATABASE "staff.db"

struct info{
	int number;  //工号
	int usertype; // 用户类型
	char name[32];  //姓名
	char pass[32];  //密码
	int age;     //年龄
	char phone[32]; //电话
	char addr[32];  //住址
    char work[32];  //职位
	char date[32];  //入职日期
	int level;      //评级水平
	float salary;    //薪资
};
 

typedef struct {
	int usertype;   //用户类型  1--管理员  0--普通用户
	int cmdtype;    //命令种类
	char data[64];  //用于传递信息
	struct info st;  //员工信息
}MSG;

#endif
