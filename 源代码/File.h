#pragma once
#include"header.h"
#include"Inode.h"

enum OpenFileMode {
	FREAD = 0x1,
	FWRITE = 0x2,
};

/*
内存打开文件结构，包含文件对应的inode和inode号，文件打开方式
*/
class File{
public:
	Inode*	m_pinode;	//Inode 指针
	int		m_inode_id;	//Inode id
	int		m_fmode;	//文件打开方式
	int		m_foffset;	//文件读写指针
public:
	File();
	~File();
	void setF(Inode* pinode, int fmode);
};

/*
OpenFiles，类，给User用
user提供文件句柄，OpenFiles提供文件指针
*/
class OpenFiles {
private:
	File* m_FileTable[USER_OPENFILE_NUM] = { 0 };
public:
	OpenFiles();
	~OpenFiles();
	//根据文件句柄fd获取文件指针
	File* GetF(int fd);
	//将文件句柄fd和文件指针绑定
	void SetF(int fd, File* pfile);
	//申请fd
	int AllocFd();
	//释放fd
	void FreeFd(int fd);
	//检查是否有文件打开
	bool has_openfile();
};

/*
OpenFileTable,类，给FileManager用，
inode转换成File结构，需要从OpenFileTable中取一项
*/
class OpenFileTable {
private:
	File	m_File[ALL_OPENFILE_NUM];
public:
	OpenFileTable();
	~OpenFileTable();
	//分配一个空闲File项
	File* Falloc();
	//回收一个空闲File项
	void Ffree(File* pfile);
	//寻找文件是否已打开，若已打开实现文件Inode共用
	Inode* Ifind(int inode_id);
};

extern OpenFileTable* g_ftable;