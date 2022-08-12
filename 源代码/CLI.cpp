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
	cout << "����ģʽ�����ú���test0" << endl;
	cout << "�Ƿ�Ҫ���ԣ�" << endl;
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
	cout << "�Ƿ�Ҫ���ļ�ϵͳ����Ĭ�ϳ�ʼ��(��Ӧ���������򲿷�)��[y/n]" << endl;
	char c;
	cin >> c;
	if (c != 'y')
		return;
	cout << "Ĭ�ϳ�ʼ����ʼ��" << endl;
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
	cout << "Ĭ�ϳ�ʼ��������" << endl;

}

void interaction() {
	cout << "Ljyux OS" << endl;
	cout << "Version:1.0.0" << endl;
	cout << "by: Lin Jiayi, 2019CS , Tongji University" << endl;
	cout << "��¼�׶Σ��밴esc�˳�����" << endl;
	string command;
	while (os->login() == false) 
		continue;
	//test0();
	initialize();
	while (getchar() != '\n') continue;
	cout << endl;
	cout << "����help�����Ի����ʾ������exit�������˳�" << endl;
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
					cout << "�����̿鲻�㣡" << endl;
					break;
				case 2:
					cout << "���д���Inode���㣡" << endl;
					break;
				default:
					cout << "��������֪����" << endl;
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
	cout << "����ָ�" << endl;
	helplist();
	cout << endl;
	cout << "ָ��˵��:" << endl;
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