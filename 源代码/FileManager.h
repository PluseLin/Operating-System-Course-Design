#pragma once
#include"header.h"
#include"FileSystem.h"
#include"File.h"
#include"DiskDevice.h"
#include"Inode.h"

enum SearchDirStatus {
	DIR_EXIST = 1,
	FATHER_NOT_EXIST = -1,
	CURDIR_NOT_EXIST = -2
};

/*目录文件项*/
struct DirEntry {
	int		m_inode;				//目录的inode号
	char	m_dname[DIR_MAXLEN + 1];//目录名
};

/*便于目录文件读写而包装的结构体*/
struct DirBlock {
	DirEntry	m_entry[DIR_NUM];
};

/*
FileManager类，负责文件层的操作
提供包括路径搜索，文件打开关闭，文件创建，文件读写
以及目录读写等函数的接口供Ljyux使用。
同时存有内存打开结构表指针和文件系统类指针
*/
class FileManager{
private:
	FileSystem*		m_pfs;		//FileSystem指针
	OpenFileTable*	m_ptable;	//打开文件表指针
	InodeTable* m_itable;		//InodeTable指针
	int				m_RootInodeid;
	int				m_curInodeid;
private:
	/*目录相关函数的辅助函数*/
	//目录文件的搜索，若找不到，DirEntry中inode号为-1
	DirEntry search(Inode* inode, string name, int type = DIRECTORY_FILE);
	//目录文件中写入 . 目录项
	void mk_curdir(Inode* inode);
	//目录文件中写入 .. 目录项
	void mk_lastdir(Inode* inode, int inode_id);
	//判断一个文件是否为空文件
	bool inode_is_empty(Inode* inode);
	//在目录文件中寻找目录项
	//int SearchInodeid(Inode* inode, string dir);
public:
	FileManager();
	~FileManager();
	/*初始化相关*/
	//格式化，包括建立根目录文件
	void fformat();

	/*目录相关*/
	//获取全部目录项
	vector<DirEntry> getDirs(Inode* inode);
	//目录搜索，返回路径对应的Inode，搜索错误性以引用方式返回
	Inode* NameI(string fp, int& status,int type=DIRECTORY_FILE);
	//在目录文件中添加一个目录
	void AddDirEntry(Inode* inode, string dir);
	//删去目录文件中的一个目录
	void RemoveDirEntry(Inode* inode, int del_id);
	//列出目录所有文件名
	void ls_(Inode* inode);
	//列出目录所有文件的具体信息
	void ls_l(Inode* inode);
	//修改当前inode
	void chinode(Inode* inode);
	/*数据文件读写相关*/
	//创建文件
	Inode* fcreate(int imode);
	//数据文件添加与删除
	void AddFile(Inode* father, Inode* child, string name);
};

