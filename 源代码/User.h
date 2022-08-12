#pragma once
#include"header.h"
#include"Inode.h"
#include"File.h"



/*
UserBlock，存入内存中，共512B，存入0#
本实验允许最多用户为16个，组内用户数不限制
*/
struct UserBlock {
	UserInfo	m_user[USER_MAXNUM];
};

/*
User结构体，Ljyux所使用的的结构
存放u_id,g_id，和打开文件表
*/
class User{
public:
	UserInfo* m_uinfo;
	OpenFiles* m_files;	//打开文件目录表
	map<int, int>	m_inodemap;//inode_id -> fd
public:
	User();
	~User();
	void clear();
	//uid和gid设置
	void setuid(short uid);
	void setgid(short gid);
	//fd设置读取
	int getfd(int inode_id);
	void setfd(int fd, int inode_id);
	void freefd(int inode_id);
	//文件打开和释放，主要是文件句柄处理
	//int fopen();
	//void fclose();
};

class UserTable {
public:
	User	m_Users[USER_MAXNUM];
public:
	UserTable();
	~UserTable();

	void clear();
	void reload();

	User* UGet(string uname);
	User* UGet(int uid);
	User* UAlloc();
	void UDelete(User* user);
	vector<User*> GetAllUser();
};

extern UserTable* g_utable;
