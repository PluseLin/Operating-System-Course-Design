#pragma once
#include"Ljyux.h"
#include"helper.h"

typedef void (*help_func)();

//格式化
void mkfs	(Ljyux& os, vector<string>& tokens);
//目录相关
void mkdir(Ljyux& os, vector<string>& tokens);
void rmdir(Ljyux& os, vector<string>& tokens);
void ls(Ljyux& os, vector<string>& tokens);
void cd(Ljyux& os, vector<string>& tokens);
//文件相关
void mkfile(Ljyux& os, vector<string>& tokens);
void rmfile(Ljyux& os, vector<string>& tokens);
void fopen(Ljyux& os, vector<string>& tokens);
void fclose(Ljyux& os, vector<string>& tokens);
void fread(Ljyux& os, vector<string>& tokens);
void fwrite(Ljyux& os, vector<string>& tokens);
void fseek(Ljyux& os, vector<string>& tokens);
void cat(Ljyux& os, vector<string>& tokens);

void load(Ljyux& os, vector<string>& tokens);
void dump(Ljyux& os, vector<string>& tokens);

//用户相关
void whoami(Ljyux& os, vector<string>& tokens);
void adduser(Ljyux& os, vector<string>& tokens);	//仅root
void deluser(Ljyux& os, vector<string>& tokens);	//仅root
void su(Ljyux& os, vector<string>& tokens);
void setgroup(Ljyux& os, vector<string>& tokens);	//仅root
void lsuser(Ljyux& os, vector<string>& tokens);
//权限相关
void chmod(Ljyux& os, vector<string>& tokens);
void setmod(Ljyux& os, vector<string>& tokens);
//展示盘块
void lsinode(Ljyux& os, vector<string>& tokens);	//仅root
void lsblk(Ljyux& os, vector<string>& tokens);		//仅root
//测试
void test(Ljyux& os, vector<string>& tokens);
//帮助
void help(Ljyux& os, vector<string>& tokens);