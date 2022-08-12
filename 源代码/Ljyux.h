#pragma once
#include"header.h"
#include"DiskDevice.h"
#include"FileSystem.h"
#include"FileManager.h"
#include"Inode.h"
#include"File.h"
#include"User.h"


class Ljyux{
public:
	FileManager*	m_pfm;			//FileManager指针
	//全局变量引用指针
	FileSystem*		m_fs;			//FileSystem指针
	DiskDevice*		m_Disk;			//DiskDevice指针
	OpenFileTable*	m_ftable;		//OpenFileTable指针
	InodeTable*		m_itable;		//InodeTable指针
	UserTable*		m_utable;		//UserTable指针

	string			m_curdir;		//当前工作目录
	User*			m_user;			//当前用户		

private:
	bool is_root(User* user);
	bool has_authroity(Inode* inode, int umode);
public:
	/*构造与释放*/
	void initialize_DiskDevice();
	void destruct_DiskDevice();
	void initialize_global();
	void destruct_global();
	Ljyux();
	~Ljyux();

	string curdir();

	/*文件操作接口*/
	//登录
	bool login();

	//格式化
	void fformat();
	//目录相关
	void mkdir(string fp);
	void rmdir(string fp);
	void ls(string fp);
	void ls_l(string fp);
	void lsinode(int inode_id);
	void lsblk(int blk_id);
	void cd(string fp);
	//文件相关
	void fcreate(string fp, int imode);
	void fdelete(string fp);
	void fopen(string fp, int fmode);
	void fclose(string fp);
	void fread(string fp, int rsize, string& str);
	void fwrite(string fp, string src,int wsize);
	void fseek(string fp, int mode, int offset);
	void cat(string fp);
	void load(string src, string dst);
	void dump(string src, string dst);
	//权限相关
	void chmod(string src, int mode, bool is_add);
	void setmod(string src, int mode);
	//用户相关
	void whoami();
	void adduser(string uname, string pswd);
	void deluser(string uname);
	void su(string uname);
	void setgroup(string uname, int gid);
	void lsuser();

};

