#pragma once
#include"Ljyux.h"
#include"helper.h"

typedef void (*help_func)();

//��ʽ��
void mkfs	(Ljyux& os, vector<string>& tokens);
//Ŀ¼���
void mkdir(Ljyux& os, vector<string>& tokens);
void rmdir(Ljyux& os, vector<string>& tokens);
void ls(Ljyux& os, vector<string>& tokens);
void cd(Ljyux& os, vector<string>& tokens);
//�ļ����
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

//�û����
void whoami(Ljyux& os, vector<string>& tokens);
void adduser(Ljyux& os, vector<string>& tokens);	//��root
void deluser(Ljyux& os, vector<string>& tokens);	//��root
void su(Ljyux& os, vector<string>& tokens);
void setgroup(Ljyux& os, vector<string>& tokens);	//��root
void lsuser(Ljyux& os, vector<string>& tokens);
//Ȩ�����
void chmod(Ljyux& os, vector<string>& tokens);
void setmod(Ljyux& os, vector<string>& tokens);
//չʾ�̿�
void lsinode(Ljyux& os, vector<string>& tokens);	//��root
void lsblk(Ljyux& os, vector<string>& tokens);		//��root
//����
void test(Ljyux& os, vector<string>& tokens);
//����
void help(Ljyux& os, vector<string>& tokens);