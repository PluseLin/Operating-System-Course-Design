#pragma once
#include"header.h"
#include"Inode.h"
#include"File.h"



/*
UserBlock�������ڴ��У���512B������0#
��ʵ����������û�Ϊ16���������û���������
*/
struct UserBlock {
	UserInfo	m_user[USER_MAXNUM];
};

/*
User�ṹ�壬Ljyux��ʹ�õĵĽṹ
���u_id,g_id���ʹ��ļ���
*/
class User{
public:
	UserInfo* m_uinfo;
	OpenFiles* m_files;	//���ļ�Ŀ¼��
	map<int, int>	m_inodemap;//inode_id -> fd
public:
	User();
	~User();
	void clear();
	//uid��gid����
	void setuid(short uid);
	void setgid(short gid);
	//fd���ö�ȡ
	int getfd(int inode_id);
	void setfd(int fd, int inode_id);
	void freefd(int inode_id);
	//�ļ��򿪺��ͷţ���Ҫ���ļ��������
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
