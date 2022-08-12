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

/*Ŀ¼�ļ���*/
struct DirEntry {
	int		m_inode;				//Ŀ¼��inode��
	char	m_dname[DIR_MAXLEN + 1];//Ŀ¼��
};

/*����Ŀ¼�ļ���д����װ�Ľṹ��*/
struct DirBlock {
	DirEntry	m_entry[DIR_NUM];
};

/*
FileManager�࣬�����ļ���Ĳ���
�ṩ����·���������ļ��򿪹رգ��ļ��������ļ���д
�Լ�Ŀ¼��д�Ⱥ����Ľӿڹ�Ljyuxʹ�á�
ͬʱ�����ڴ�򿪽ṹ��ָ����ļ�ϵͳ��ָ��
*/
class FileManager{
private:
	FileSystem*		m_pfs;		//FileSystemָ��
	OpenFileTable*	m_ptable;	//���ļ���ָ��
	InodeTable* m_itable;		//InodeTableָ��
	int				m_RootInodeid;
	int				m_curInodeid;
private:
	/*Ŀ¼��غ����ĸ�������*/
	//Ŀ¼�ļ������������Ҳ�����DirEntry��inode��Ϊ-1
	DirEntry search(Inode* inode, string name, int type = DIRECTORY_FILE);
	//Ŀ¼�ļ���д�� . Ŀ¼��
	void mk_curdir(Inode* inode);
	//Ŀ¼�ļ���д�� .. Ŀ¼��
	void mk_lastdir(Inode* inode, int inode_id);
	//�ж�һ���ļ��Ƿ�Ϊ���ļ�
	bool inode_is_empty(Inode* inode);
	//��Ŀ¼�ļ���Ѱ��Ŀ¼��
	//int SearchInodeid(Inode* inode, string dir);
public:
	FileManager();
	~FileManager();
	/*��ʼ�����*/
	//��ʽ��������������Ŀ¼�ļ�
	void fformat();

	/*Ŀ¼���*/
	//��ȡȫ��Ŀ¼��
	vector<DirEntry> getDirs(Inode* inode);
	//Ŀ¼����������·����Ӧ��Inode�����������������÷�ʽ����
	Inode* NameI(string fp, int& status,int type=DIRECTORY_FILE);
	//��Ŀ¼�ļ������һ��Ŀ¼
	void AddDirEntry(Inode* inode, string dir);
	//ɾȥĿ¼�ļ��е�һ��Ŀ¼
	void RemoveDirEntry(Inode* inode, int del_id);
	//�г�Ŀ¼�����ļ���
	void ls_(Inode* inode);
	//�г�Ŀ¼�����ļ��ľ�����Ϣ
	void ls_l(Inode* inode);
	//�޸ĵ�ǰinode
	void chinode(Inode* inode);
	/*�����ļ���д���*/
	//�����ļ�
	Inode* fcreate(int imode);
	//�����ļ������ɾ��
	void AddFile(Inode* father, Inode* child, string name);
};

