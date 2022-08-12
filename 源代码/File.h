#pragma once
#include"header.h"
#include"Inode.h"

enum OpenFileMode {
	FREAD = 0x1,
	FWRITE = 0x2,
};

/*
�ڴ���ļ��ṹ�������ļ���Ӧ��inode��inode�ţ��ļ��򿪷�ʽ
*/
class File{
public:
	Inode*	m_pinode;	//Inode ָ��
	int		m_inode_id;	//Inode id
	int		m_fmode;	//�ļ��򿪷�ʽ
	int		m_foffset;	//�ļ���дָ��
public:
	File();
	~File();
	void setF(Inode* pinode, int fmode);
};

/*
OpenFiles���࣬��User��
user�ṩ�ļ������OpenFiles�ṩ�ļ�ָ��
*/
class OpenFiles {
private:
	File* m_FileTable[USER_OPENFILE_NUM] = { 0 };
public:
	OpenFiles();
	~OpenFiles();
	//�����ļ����fd��ȡ�ļ�ָ��
	File* GetF(int fd);
	//���ļ����fd���ļ�ָ���
	void SetF(int fd, File* pfile);
	//����fd
	int AllocFd();
	//�ͷ�fd
	void FreeFd(int fd);
	//����Ƿ����ļ���
	bool has_openfile();
};

/*
OpenFileTable,�࣬��FileManager�ã�
inodeת����File�ṹ����Ҫ��OpenFileTable��ȡһ��
*/
class OpenFileTable {
private:
	File	m_File[ALL_OPENFILE_NUM];
public:
	OpenFileTable();
	~OpenFileTable();
	//����һ������File��
	File* Falloc();
	//����һ������File��
	void Ffree(File* pfile);
	//Ѱ���ļ��Ƿ��Ѵ򿪣����Ѵ�ʵ���ļ�Inode����
	Inode* Ifind(int inode_id);
};

extern OpenFileTable* g_ftable;