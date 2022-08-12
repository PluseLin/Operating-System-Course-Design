#pragma once
/*ͷ�ļ�*/
#include<iostream>
#include<iomanip>
#include<fstream>
#include<cstring>
#include<vector>
#include<string>
#include<map>
#include<initializer_list>
#include<stdexcept>
#include<cstdlib>
#include<conio.h>

using namespace std;

/*�궨��*/
/*�̿����*/
#define BLOCK_SIZE			512 
#define	DISK_BLOCK_NUM		65536
//OSԤ������
#define OS_BLOCK_START		0
#define OS_BLOCK_NUM		2
//SPB�鲿��
#define	SPB_BLOCK_START		2
#define SPB_BLOCK_NUM		2
//INODE�鲿��
#define	INODE_BLOCK_START	4
#define INODE_BLOCK_NUM		128
#define INODE_SIZE			32
#define INODE_MAXNUM		1024


//DATA�鲿��
#define DATA_BLOCK_START	132
#define BLOCK_GROUP_SIZE	100

/*Buffer���*/
#define	BUFFER_NUM			16

/*�����*/
//Openfile�ṹ���
#define USER_OPENFILE_NUM	16
#define ALL_OPENFILE_NUM	128	

//Ŀ¼�����
#define DIR_MAXLEN			59
#define DIR_NUM				16


//�û���Ϣ���
#define	USERNAME_MAXLEN		13
#define	PASSWD_MAXLEN		13
#define USER_MAXNUM			16

//�ļ���д���
#define	FSEEK_BEGIN			0
#define	FSEEK_END			1
#define	FSEEK_ADD			2

/*typedef*/
typedef unsigned int u_int;
typedef unsigned char u_char;

/*һЩ������ȫ�ֺ���*/
//���ַ�sep�ָ��ַ���
vector<string> split(string str, char sep);

//ȡ·�����ϸ���Ŀ¼
vector<string> split_fp(string fp);

//ȡ���ļ�����Ŀ¼�����һ�
string get_filename(string fp);

//ƴ���ļ���
string join(string fp1, string fp2);

//��ȡ���Ե�ַ
string abspath(string fp);

//�ӱ��ض�ȡ�ļ�
string readfile(string fp);

//�򱾵�д���ļ�
void writefile(string fp, string str);

//��ȡ����������
string CmdInput();

//�ַ���ת����
int to_int(string str, int base = 10);

//��ȡ���ڵ�·��
string get_father(string fp);

//bool file_exist(string fp);