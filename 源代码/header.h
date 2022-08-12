#pragma once
/*头文件*/
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

/*宏定义*/
/*盘块相关*/
#define BLOCK_SIZE			512 
#define	DISK_BLOCK_NUM		65536
//OS预留部分
#define OS_BLOCK_START		0
#define OS_BLOCK_NUM		2
//SPB块部分
#define	SPB_BLOCK_START		2
#define SPB_BLOCK_NUM		2
//INODE块部分
#define	INODE_BLOCK_START	4
#define INODE_BLOCK_NUM		128
#define INODE_SIZE			32
#define INODE_MAXNUM		1024


//DATA块部分
#define DATA_BLOCK_START	132
#define BLOCK_GROUP_SIZE	100

/*Buffer相关*/
#define	BUFFER_NUM			16

/*类相关*/
//Openfile结构相关
#define USER_OPENFILE_NUM	16
#define ALL_OPENFILE_NUM	128	

//目录名相关
#define DIR_MAXLEN			59
#define DIR_NUM				16


//用户信息相关
#define	USERNAME_MAXLEN		13
#define	PASSWD_MAXLEN		13
#define USER_MAXNUM			16

//文件读写相关
#define	FSEEK_BEGIN			0
#define	FSEEK_END			1
#define	FSEEK_ADD			2

/*typedef*/
typedef unsigned int u_int;
typedef unsigned char u_char;

/*一些辅助性全局函数*/
//按字符sep分割字符串
vector<string> split(string str, char sep);

//取路径名上各级目录
vector<string> split_fp(string fp);

//取出文件名（目录中最后一项）
string get_filename(string fp);

//拼接文件名
string join(string fp1, string fp2);

//获取绝对地址
string abspath(string fp);

//从本地读取文件
string readfile(string fp);

//向本地写入文件
void writefile(string fp, string str);

//获取输入区内容
string CmdInput();

//字符串转数字
int to_int(string str, int base = 10);

//获取父节点路径
string get_father(string fp);

//bool file_exist(string fp);