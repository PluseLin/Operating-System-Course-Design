#include"Ljyux.h"
#include"CommandParser.h"
#include<stdio.h>

typedef void(*command)(Ljyux&, vector<string>&);

Ljyux* os = nullptr;

map<string, command> cmap{
	{"mkfs",mkfs},
	{"mkdir",mkdir},
	{"rmdir",rmdir},
	{"ls",ls},
	{"cd",cd},
	{"mkfile",mkfile},
	{"rmfile",rmfile},
	{"fopen",fopen},
	{"fclose",fclose},
	{"fread",fread},
	{"fwrite",fwrite},
	{"fseek",fseek},
	{"cat",cat},
	{"load",load},
	{"dump",dump},
	{"whoami",whoami},
	{"adduser",adduser},
	{"deluser",deluser},
	{"su",su},
	{"setgroup",setgroup},
	{"lsinode",lsinode},
	{"lsblk",lsblk},
	{"test",test},
	{"lsuser",lsuser},
	{"chmod",chmod},
	{"setmod",setmod},
	{"help",help},
};

void test0() {
	cout << "测试模式，调用函数test0" << endl;
	cout << "是否要测试？" << endl;
	string c; cin >> c;
	if (c != "y") return;
	vector<string> commands{
		"mkfs",
		"mkfile 11.pdf",
		"fopen 11.pdf",
		"lsblk 4",
		"load D:\\11.pdf 11.pdf",
		"dump 11.pdf D:\\22.pdf"
	};
	for (auto& cmd : commands) {
		vector<string> tokens = split(cmd, ' ');
		if (tokens[0] == "load") {
			cout << "load" << endl;
		}
		auto ptr = cmap.find(tokens[0]);
		if (ptr == cmap.end())
			continue;
		ptr->second(*os, tokens);
		cout << endl;
	}
	while (getchar() != '\n') continue;
}

void exit_process() {
	delete os;
}

void initialize() {
	cout << "是否要对文件系统进行默认初始化(对应课设主程序部分)？[y/n]" << endl;
	char c;
	cin >> c;
	if (c != 'y')
		return;
	cout << "默认初始化开始！" << endl;
	vector<string> commands{
		"mkfs",
		"mkdir /bin",
		"mkdir /etc",
		"mkdir /home",
		"mkdir /dev",
		"mkdir /home/texts",
		"mkdir /home/reports",
		"mkdir /home/photos",
		"cd /home/texts",
		"mkfile report.pdf",
		"fopen report.pdf",
		"load .\\file\\report.pdf report.pdf",
		"fclose report.pdf",
		"cd /home/reports",
		"mkfile readme.txt",
		"fopen readme.txt",
		"load .\\file\\readme.txt readme.txt",
		"fclose readme.txt",
		"cd /home/photos",
		"mkfile pic.jpg",
		"fopen pic.jpg",
		"load .\\file\\pic.jpg pic.jpg",
		"fclose pic.jpg",
		"cd /"
	};
	for (auto& command : commands) {
		vector<string> tokens = split(command, ' ');
		auto ptr = cmap.find(tokens[0]);
		if (ptr == cmap.end())
			continue;
		ptr->second(*os, tokens);
	}
	cout << "默认初始化结束！" << endl;

}

void interaction() {
	cout << "Ljyux OS" << endl;
	cout << "Version:1.0.0" << endl;
	cout << "by: Lin Jiayi, 2019CS , Tongji University" << endl;
	cout << "登录阶段，请按esc退出程序" << endl;
	string command;
	while (os->login() == false) 
		continue;
	//test0();
	initialize();
	while (getchar() != '\n') continue;
	cout << endl;
	cout << "输入help命令以获得提示，输入exit以正常退出" << endl;
	while (true) {
		cout << os->curdir() << " > ";
		getline(cin, command);
		vector<string> tokens = split(command, ' ');
		if (tokens.size() == 0)
			continue;
		if (tokens[0] == "exit")
			break;
		auto ptr = cmap.find(tokens[0]);
		if (ptr == cmap.end()) 
			continue;
		try {
			ptr->second(*os, tokens);
		}
		catch (int& e) {
			switch (e){
				case 1:
					cout << "空闲盘块不足！" << endl;
					break;
				case 2:
					cout << "空闲磁盘Inode不足！" << endl;
					break;
				default:
					cout << "其他不可知错误" << endl;
					break;
			}
		}
	}
	cout << endl;
}

void init_readme() {
	auto f=freopen("readme.txt", "w", stdout);
	map<string, help_func> hmap{
		{"mkfs",mkfs_help},
		{"mkdir",mkdir_help},
		{"rmdir",rmdir_help},
		{"ls",ls_help},
		{"cd",cd_help},
		{"mkfile",mkfile_help},
		{"rmfile",rmfile_help},
		{"fopen",fopen_help},
		{"fclose",fclose_help},
		{"fread",fread_help},
		{"fwrite",fwrite_help},
		{"fseek",fseek_help},
		{"cat",cat_help},
		{"load",load_help},
		{"dump",dump_help},
		{"whoami",whoami_help},
		{"adduser",adduser_help},
		{"deluser",deluser_help},
		{"su",su_help},
		{"setgroup",setgroup_help},
		{"lsinode",lsinode_help},
		{"lsblk",lsblk_help},
		{"lsuser",lsuser_help},
		{"chmod",chmod_help},
		{"setmod",setmod_help},
		{"help",help_help},
	};
	cout << "可用指令：" << endl;
	helplist();
	cout << endl;
	cout << "指令说明:" << endl;
	for (auto& pr : hmap) {
		pr.second();
		cout << endl;
	}
}

int main() {
	os = new Ljyux;
	atexit(exit_process);
	interaction();
	//init_readme();
	return 0;
}

/*
mkdir /test
mkfile /test/Jerry
fopen /test/Jerry
fwrite /test/Jerry -spyth
fseek /test/Jerry -b 500
fwrite /test/Jerry -s
*/

/*
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
*/