#pragma once
#include"header.h"
#include"DiskDevice.h"
#include"FileSystem.h"



/*
Inode，类，内存中inode的表示，考虑到功能的添加
内存Inode会不同于磁盘Inode，因此单独拎出来
*/

class Inode{
private:
	FileSystem*& m_pfs=g_fs;		//FileSystem指针（实为全局变量指针）
	DiskDevice*& m_Disk=g_Disk;		//DiskDevice指针
public:
	int			m_count = 0;//引用计数
	int			m_id = -1;	//磁盘Inode的id号
	DiskInode* m_pinode;	//磁盘Inode指针

	
public:
	Inode();
	Inode(int inode_id);
	Inode(const Inode& inode);
	~Inode();
	//Inode读出与写入
	void load(int inode_id = -1);
	void dump(int inode_id = -1);
	//盘块映射
	int Bmap(int lbn);
	//读Inode，返回实际读字节
	int ReadI(char* dst,int offset,int size);
	//写Inode，返回实际写字节
	int WriteI(char* src, int offset, int size);
	//inode盘块回收
	void FreeDisk();
	//inode权限展示
	string authority();
};

/*
InodeTable,类，用于管理内存Inode
*/
class InodeTable {
private:
	static const int NINODE = 100;
	Inode	m_inodes[NINODE];
public:
	InodeTable();
	~InodeTable();
	void clear();
	Inode* IFind(int inode_id);
	Inode* IGet(int inode_id);
	void IPut(Inode* pinode);
};

extern InodeTable* g_itable;