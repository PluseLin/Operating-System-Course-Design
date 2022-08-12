#pragma once
#include"header.h"
#include"DiskDevice.h"
#include"FileSystem.h"



/*
Inode���࣬�ڴ���inode�ı�ʾ�����ǵ����ܵ����
�ڴ�Inode�᲻ͬ�ڴ���Inode����˵��������
*/

class Inode{
private:
	FileSystem*& m_pfs=g_fs;		//FileSystemָ�루ʵΪȫ�ֱ���ָ�룩
	DiskDevice*& m_Disk=g_Disk;		//DiskDeviceָ��
public:
	int			m_count = 0;//���ü���
	int			m_id = -1;	//����Inode��id��
	DiskInode* m_pinode;	//����Inodeָ��

	
public:
	Inode();
	Inode(int inode_id);
	Inode(const Inode& inode);
	~Inode();
	//Inode������д��
	void load(int inode_id = -1);
	void dump(int inode_id = -1);
	//�̿�ӳ��
	int Bmap(int lbn);
	//��Inode������ʵ�ʶ��ֽ�
	int ReadI(char* dst,int offset,int size);
	//дInode������ʵ��д�ֽ�
	int WriteI(char* src, int offset, int size);
	//inode�̿����
	void FreeDisk();
	//inodeȨ��չʾ
	string authority();
};

/*
InodeTable,�࣬���ڹ����ڴ�Inode
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