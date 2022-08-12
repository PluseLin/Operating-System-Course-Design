#include "CommandParser.h"
void mkfs(Ljyux& os, vector<string>& tokens) {
	os.fformat();
	cout << "格式化完毕！" << endl;
}

void mkdir(Ljyux& os, vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "命令格式错误，请检查！" << endl;
		return;
	}
	os.mkdir(tokens[1]);
}

void rmdir(Ljyux& os, vector<string>& tokens) {
	//rmdir xxx
	string fp = "";
	for (auto& token : tokens) {
		if (token == "rmdir") continue;
		else if (fp == "") fp = token;
	}
	if (fp != "") {
		os.rmdir(fp);
	}
}

void ls(Ljyux& os, vector<string>& tokens) {
	bool _l = false;
	string fp = "";
	//fp接收第一个不以-开头的字符串
	for (auto& token : tokens) {
		if (token == "ls") continue;
		if (token == "-l") _l = true;
		else if (fp == "") fp = token;
	}
	if (fp == "")
		return;
	if (_l) {
		os.ls_l(fp);
	}
	else {
		os.ls(fp);
	}
}

void lsinode(Ljyux& os, vector<string>& tokens) {
	int inode_id = -1;
	for (auto& token : tokens) {
		if (token == "lsinode") continue;
		else if (inode_id == -1) {
			inode_id = to_int(token);
		}
	}
	if (inode_id != -1) {
		os.lsinode(inode_id);
	}
}

void lsblk(Ljyux& os, vector<string>& tokens) {
	int blk_id = -1;
	for (auto& token : tokens) {
		if (token == "lsblk") continue;
		else if (blk_id == -1) {
			blk_id = to_int(token);
		}
	}
	os.lsblk(blk_id);
}

void cd(Ljyux& os, vector<string>& tokens) {
	string fp = "";
	for (auto& token : tokens) {
		if (token == "cd") continue;
		else if (fp == "") fp = token;
	}
	if (fp != "")
		os.cd(fp);
}

void mkfile(Ljyux& os, vector<string>& tokens) {
	//mkfile xxx [-m 777]

	string fp = "";
	int imode = 0x777;
	for (u_int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "mkfile") continue;
		if (tokens[i] == "-m" && i + 1 < tokens.size()) {
			imode = to_int(tokens[i + 1], 16) & 0x777;
			i++;
		}
		else if (fp == "") {
			fp = tokens[i];
		}
	}
	if (fp != "") {
		os.fcreate(fp, imode);
	}
}

void rmfile(Ljyux& os, vector<string>& tokens) {
	//rmfile xxx
	string fp = "";
	for (u_int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "rmfile") continue;
		else if (fp == "") {
			fp = tokens[i];
		}
	}
	if (fp != "") {
		os.fdelete(fp);
	}
}

void fopen(Ljyux& os, vector<string>& tokens) {
	string fp = "";
	int mode = OpenFileMode::FREAD | OpenFileMode::FWRITE;
	for (auto& token : tokens) {
		if (token == "fopen") continue;
		else if (token == "-rw") mode = OpenFileMode::FREAD | OpenFileMode::FWRITE;
		else if (token == "-ro") mode = OpenFileMode::FREAD;
		else if (fp == "") fp = token;
	}
	if (fp != "") {
		os.fopen(fp, mode);
	}
}

void fclose(Ljyux& os, vector<string>& tokens) {
	//fclose xxx
	string fp = "";
	for (auto& token : tokens) {
		if (token == "fclose") continue;
		else if (fp == "") fp = token;
	}
	if (fp != "") {
		os.fclose(fp);
	}
}

void fread(Ljyux& os, vector<string>& tokens) {
	//要求严格固定格式
	//fread	xxx [-n xxx]
	string fp = "";
	int size = 0x7fffffff;
	string str = "";
	for (u_int i = 0; i < tokens.size();i++) {
		if (tokens[i] == "fread") continue;
		if (tokens[i] == "-n" && i + 1 < tokens.size()) {
			size = to_int(tokens[i + 1]);
			i++;
		}
		else if(fp=="") {
			fp = tokens[i];
		}
	}
	if (fp != "") {
		os.fread(fp, size, str);
		if (str == "") 
			return;
		else{
			cout << "读取到了 " << str.length() << "个字节" << endl;
			cout << str << endl;
		}
	}
	
}

void fwrite(Ljyux& os, vector<string>& tokens) {
	//要求严格固定格式
	//fwrite xxx -s xxx [-n size]	(只支持单行不含空格输入)
	//fwrite xxx -i [-n size]		(自由输入，以单行#作为结尾)
	string fp = "";
	string src = "";
	int size = 0x7fffffff;
	for (u_int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "fwrite") continue;
		if (tokens[i] == "-n" && i + 1 < tokens.size()) {
			size = to_int(tokens[i + 1]);
			i++;
		}
		else if (tokens[i] == "-s" && i + 1 < tokens.size()) {
			src = (src == "") ? tokens[i + 1] : src;
			i++;
		}
		else if (tokens[i] == "-i") {
			cout << "请输入写入内容, 输入单行#退出" << endl;
			src = CmdInput();
		}
		else if (fp == "") {
			fp = tokens[i];
		}
	}
	if (fp != "" && src!="") {
		os.fwrite(fp, src, size);
	}
}

void fseek(Ljyux& os, vector<string>& tokens) {
	//要求严格固定格式
	//fseek xxx 			(默认回到开头)
	//fseek xxx -b size		(从开头开始向后移动若干字节)
	//fseek xxx -e size		(从结尾开始向前移动若干字节)
	//fseek xxx -a size		(附加向后移动若干字节)
	int mode = FSEEK_BEGIN;
	int offset = 0;
	string fp = "";
	for (u_int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "fseek") continue;
		else if (tokens[i] == "-b" && i + 1 < tokens.size()) {
			mode = FSEEK_BEGIN;
			offset = to_int(tokens[i + 1]);
		}
		else if (tokens[i] == "-e" && i + 1 < tokens.size()) {
			mode = FSEEK_END;
			offset = to_int(tokens[i + 1]);
		}
		else if (tokens[i] == "-a" && i + 1 < tokens.size()) {
			mode = FSEEK_ADD;
			offset = to_int(tokens[i + 1]);
		}
		else if (fp == "") {
			fp = tokens[i];
		}
	}
	if (fp != "") {
		os.fseek(fp, mode, offset);
	}

}

void cat(Ljyux& os, vector<string>& tokens) {
	//cat fp
	string fp = "";
	for (auto& token : tokens) {
		if (token == "cat") continue;
		else if (fp == "") fp = token;
	}
	if (fp != "") {
		os.cat(fp);
	}
}

void load(Ljyux& os, vector<string>& tokens) {
	//load fp1 fp2
	//fp1对应Windows下目录，fp2对应Ljyux下目录
	if (tokens.size() < 3) {
		cout << "命令格式有误！" << endl;
		return;
	}
	string src = tokens[1], dst = tokens[2];
	os.load(src, dst);
}

void dump(Ljyux& os, vector<string>& tokens) {
	//store fp1 fp2
	//fp1对应Ljyux下目录，fp2对应Windows下目录
	if (tokens.size() < 3) {
		cout << "命令格式有误！" << endl;
		return;
	}
	string src = tokens[1], dst = tokens[2];
	os.dump(src, dst);
}

void test(Ljyux& os, vector<string>& tokens) {
	BlockGroup blk;
	g_Disk->read(132, 0, &blk);
	cout << blk.g_nblock << endl;
}

void whoami(Ljyux& os, vector<string>& tokens) {
	//whoami
	os.whoami();
}

void adduser(Ljyux& os, vector<string>& tokens) {
	//adduser [-u xxx] [-p xxx]
	string uname = "", pswd = "";
	for (u_int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "adduser") continue;
		else if (tokens[i] == "-p" && i + 1 < tokens.size()) {
			pswd = tokens[i + 1];
		}
		else if (tokens[i] == "-u" && i + 1 < tokens.size()) {
			uname = tokens[i + 1];
		}
	}
	if (uname == "") {
		cout << "请输入用户名 : ";
		cin >> uname;
		if (uname.length() > USERNAME_MAXLEN) {
			uname = uname.substr(0, USERNAME_MAXLEN);
		}
	}
	if (pswd == "") {
		cout << "请输入密码 : ";
		cin >> uname;
		if (pswd.length() > PASSWD_MAXLEN) {
			pswd = pswd.substr(0, PASSWD_MAXLEN);
		}
	}
	os.adduser(uname,pswd);
}

void deluser(Ljyux& os, vector<string>& tokens) {
	//deluser [-u uname] 
	string uname = "";
	for (u_int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "deluser") continue;
		else if (tokens[i] == "-u" && i + 1 < tokens.size()) {
			uname = tokens[i + 1];
		}
	}
	if (uname != "") {
		os.deluser(uname);
	}
}

void su(Ljyux& os, vector<string>& tokens) {
	//su [- uname]
	string uname = "root";
	for (u_int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "su") continue;
		else if (tokens[i] == "-" && i + 1 < tokens.size()) {
			uname = tokens[i + 1];
		}
	}
	os.su(uname);
}

void setgroup(Ljyux& os, vector<string>& tokens) {
	//setgroup -u uname -g gid
	string uname = "";
	int gid = 0;
	for (u_int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "su") continue;
		else if (tokens[i] == "-u" && i + 1 < tokens.size()) {
			uname = tokens[i + 1];
		}
		else if (tokens[i] == "-g" && i + 1 < tokens.size()) {
			gid = to_int(tokens[i + 1]);
		}
	}
	if (uname == "") {
		cout << "未输入用户名！" << endl;
		return;
	}
	os.setgroup(uname,gid);
}

void lsuser(Ljyux& os, vector<string>& tokens) {
	os.lsuser();
}

void chmod(Ljyux& os, vector<string>& tokens) {
	//chmod fp (u|g|v)(+|-)(r|w|x)
	//不许换顺序！否则算错！
	//若目录权限没有按照要求写，则不执行
	int mode = 0;
	int member = 0;	//对应u,g,v
	int type = 0;	//对应r,w,x
	int is_add = 0;
	if (tokens.size() < 3 || tokens[2].length() != 3) {
		cout << "命令格式不正确！" << endl;
		return;
	}
	string fp = tokens[1];
	member = tokens[2][0] == 'u' ? 2 :
		tokens[2][0] == 'g' ? 1 :
		tokens[2][0] == 'o' ? 0 : -1;
	type = tokens[2][2] == 'r' ? 2 :
		tokens[2][2] == 'w' ? 1 :
		tokens[2][2] == 'x' ? 0 : -1;
	is_add = tokens[2][1] == '+' ? 1 : tokens[2][1] == '-' ? 0 : -1;
	if (member == -1 || type == -1 || is_add == -1) {
		cout << "命令格式不正确！" << endl;
		return;
	}
	mode = 1 << (member * 4 + type);
	os.chmod(fp, mode, is_add);
}

void setmod(Ljyux& os, vector<string>& tokens) {
	//setmod fp mode
	//mode为十六进制，若随便写，设为末12位(
	if (tokens.size() < 3) {
		cout << "命令格式不正确！" << endl;
		return;
	}
	string fp = tokens[1];
	int mode = to_int(tokens[2], 16) & 0x777;
	os.setmod(fp, mode);
}

void help(Ljyux& os, vector<string>& tokens) {
	//help [command]
	string command = "";
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
	for (auto& token : tokens) {
		if (token == "help") continue;
		else if (command == "") command = token;
	}
	auto func = hmap.find(command);
	if (func != hmap.end()) {
		func->second();
	}
	else {
		if (tokens.size() == 2) {
			cout << "找不到对应指令！" << endl;
		}
		helplist();
	}
}