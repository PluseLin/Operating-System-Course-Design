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
	FileManager*	m_pfm;			//FileManagerָ��
	//ȫ�ֱ�������ָ��
	FileSystem*		m_fs;			//FileSystemָ��
	DiskDevice*		m_Disk;			//DiskDeviceָ��
	OpenFileTable*	m_ftable;		//OpenFileTableָ��
	InodeTable*		m_itable;		//InodeTableָ��
	UserTable*		m_utable;		//UserTableָ��

	string			m_curdir;		//��ǰ����Ŀ¼
	User*			m_user;			//��ǰ�û�		

private:
	bool is_root(User* user);
	bool has_authroity(Inode* inode, int umode);
public:
	/*�������ͷ�*/
	void initialize_DiskDevice();
	void destruct_DiskDevice();
	void initialize_global();
	void destruct_global();
	Ljyux();
	~Ljyux();

	string curdir();

	/*�ļ������ӿ�*/
	//��¼
	bool login();

	//��ʽ��
	void fformat();
	//Ŀ¼���
	void mkdir(string fp);
	void rmdir(string fp);
	void ls(string fp);
	void ls_l(string fp);
	void lsinode(int inode_id);
	void lsblk(int blk_id);
	void cd(string fp);
	//�ļ����
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
	//Ȩ�����
	void chmod(string src, int mode, bool is_add);
	void setmod(string src, int mode);
	//�û����
	void whoami();
	void adduser(string uname, string pswd);
	void deluser(string uname);
	void su(string uname);
	void setgroup(string uname, int gid);
	void lsuser();

};

