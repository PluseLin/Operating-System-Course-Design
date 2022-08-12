#include "helper.h"
void mkfs_help() {
	string name = "mkfs";
	string func = "格式化文件卷";
	string format = "mkfs";
	string info = "格式化后，当前目录为/，用户为root";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
//目录相关
void mkdir_help() {
	string name = "mkdir";
	string func = "创建目录";
	string format = "mkdir fp (fp为路径名)";
	string info = "Ljyux不允许目录和文件重名";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void rmdir_help() {
	string name = "rmdir";
	string func = "删除目录";
	string format = "rmdir fp(fp为路径名)";
	string info = "删除目录时必须保证目录为空";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void ls_help(){
	string name = "ls";
	string func = "展示目录信息";
	string format = "ls [-l] fp (fp是目录路径)";
	string info = "fp不允许是数据文件";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void cd_help(){
	string name = "cd";
	string func = "更改当前目录";
	string format = "cd fp(fp为路径名)";
	string info = "无";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
//文件相关
void mkfile_help(){
	string name = "mkfile";
	string func = "创建文件";
	string format = "mkfile fp [-m auth](fp为文件路径, auth为文件权限，16进制表示) ";
	string info = "不允许与其他文件重名，不支持重名覆盖";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void rmfile_help(){
	string name = "rmfile";
	string func = "删除文件";
	string format = "rmdir fp(fp为文件路径)";
	string info = "删除文件时必须保证文件不被打开";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void fopen_help(){
	string name = "fopen";
	string func = "打开文件";
	string format = "fopen fp [-rw|-ro](fp为文件路径)";
	string info = "打开文件时需要保证有权限";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void fclose_help(){
	string name = "fclose";
	string func = "关闭文件";
	string format = "fclose fp(fp为文件路径)";
	string info = "必须保证文件已打开";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void fread_help(){
	string name = "fread";
	string func = "读取文件";
	string format = "fread fp [-n length](fp为文件路径，length为读取长度)";
	string info = "必须保证文件打开从读写指针开始读取length个字节到屏幕中，会修改读写指针";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void fwrite_help(){
	string name = "fwrite";
	string func = "写入文件";
	string format =
		"\nfwrite fp -s str [-n size] (fp为文件路径，str为写入内容，size为写入大小)\n"\
		"fwrite fp -i [-n size](输入模式，fp为文件路径，size为写入大小)";
	string info =
		"\n-s输入模式不支持字符串含有空格和回车等间隔符"\
		"\n-i模式下，以单行#加回车表示输入结束"\
		"\n必须保证文件已打开";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void fseek_help(){
	string name = "fseek";
	string func = "移动文件读写指针";
	string format =
		"\nfseek fp (fp为文件路径, 默认回到开头)"\
		"\nfseek fp -b size (fp为文件路径，从文件开始向后移动size字节)"\
		"\nfseek fp -e size (fp为文件路径，从文件结尾向前移动size字节)"\
		"\nfseek fp -a size (fp为文件路径，从当前位置移动size个字节)";
	string info = "必须保证文件已打开";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void cat_help(){
	string name = "cat";
	string func = "输出文件";
	string format = "cat fp(fp为文件路径)";
	string info = "必须保证文件已打开，此操作不改变读写指针";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}

void load_help(){
	string name = "load";
	string func = "从宿主机读取文件到Ljyux中";
	string format = "load src dst (src为宿主机文件路径，dst为Ljyux文件路径)";
	string info = "必须保证文件已打开，此操作不改变读写指针，会覆盖";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void dump_help(){
	string name = "dump";
	string func = "从Ljyux写出文件到宿主机中";
	string format = "dump src dst (src为Ljyux文件路径，dst为宿主机文件路径)";
	string info = "必须保证文件已打开";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}

//用户相关
void whoami_help(){
	string name = "whoami";
	string func = "查看当前用户";
	string format = "whoami";
	string info = "无";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void adduser_help(){
	string name = "adduser";
	string func = "添加用户";
	string format = "adduser [-u name] [-p password] (name为用户名,password为密码)";
	string info = "用户名和密码最长长度为13。若不添加-u -p项，则会在后续要求输入";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void deluser_help(){
	string name = "deluser";
	string func = "删除用户";
	string format = "deluser -u uname (uname为用户名)";
	string info = "必须保证用户存在。删除时保留此用户的文件";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void su_help(){
	string name = "su";
	string func = "更换用户";
	string format = "su [- uname] (uname为用户名)";
	string info = "若不指明用户，则切换到root用户";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void setgroup_help(){
	string name = "setgroup";
	string func = "为用户设置分组";
	string format = "setgroup -u uname -g gid (uname为用户名，gid为id号，要求在0-15，若错误则默认为0)";
	string info = "必须指明用户和gid";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void lsuser_help(){
	string name = "lsuser";
	string func = "展示用户";
	string format = "lsuser";
	string info = "无";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
//权限相关
void chmod_help(){
	string name = "chmod";
	string func = "更改文件权限";
	string format = "chmod fp (u|g|v)(+|-)(r|w|x) (fp为文件路径)";
	string info = "类似Linux的chmod命令";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void setmod_help(){
	string name = "setmod";
	string func = "设置文件权限";
	string format = "setmod fp mode (fp为文件路径，mode为文件权限，16进制)";
	string info = "权限设置参考Unix/Linux文件权限，若对文件权限不熟悉不建议使用此命令";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
//展示盘块
void lsinode_help(){
	string name = "lsinode";
	string func = "展示inode信息";
	string format = "lsinode inode_id (inode_id是inode号)";
	string info = "无";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void lsblk_help(){
	string name = "lsblk";
	string func = "展示盘块";
	string format = "lsblk blk_id (blk_id是盘块号)";
	string info = "无";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}
void help_help() {
	string name = "help";
	string func = "展示帮助信息";
	string format = "help [command] (command为命令)";
	string info = "若不指明命令，或命令有误，则会展示所有可用命令";

	cout << "命令名称: " << name << endl;
	cout << "命令格式: " << format << endl;
	cout << "命令作用: " << func << endl;
	cout << "命令说明: " << info << endl;
}

//命令list
void helplist(){
	vector<string>commands{
		"mkfs","格式化文件卷",
		"mkdir","创建目录",
		"rmdir","删除目录",
		"ls","展示目录信息",
		"cd","更改当前目录",
		"mkfile","创建文件",
		"rmfile","删除文件",
		"fopen","打开文件",
		"fclose","关闭文件",
		"fread","读取文件",
		"fwrite","写入文件",
		"fseek","移动文件读写指针",
		"cat","输出文件",
		"load","从宿主机读取文件到Ljyux中",
		"dump","从Ljyux写出文件到宿主机中" ,
		"whoami","查看当前用户",
		"adduser","添加用户",
		"deluser","删除用户",
		"su","更换用户",
		"setgroup","为用户设置分组",
		"lsinode","展示inode信息",
		"lsblk","展示盘块",
		"help","展示帮助信息",
		"lsuser","展示用户",
		"chmod","更改文件权限",
		"setmod","设置文件权限",

	};
	cout << "可用指令如下: " << endl;
	for (u_int i = 0; i < commands.size() - 1; i += 2) {
		cout << commands[i] << " : " << commands[i + 1] << endl;
	}
}