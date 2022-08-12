#pragma once
#include"header.h"
void mkfs_help();
//目录相关
void mkdir_help();
void rmdir_help();
void ls_help();
void cd_help();
//文件相关
void mkfile_help();
void rmfile_help();
void fopen_help();
void fclose_help();
void fread_help();
void fwrite_help();
void fseek_help();
void cat_help();

void load_help();
void dump_help();

//用户相关
void whoami_help();
void adduser_help();
void deluser_help();
void su_help();
void setgroup_help();
void lsuser_help();
//权限相关
void chmod_help();
void setmod_help();
//展示盘块
void lsinode_help();
void lsblk_help();

void help_help();
//命令list
void helplist();

