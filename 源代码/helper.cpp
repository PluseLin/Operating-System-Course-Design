#include "helper.h"
void mkfs_help() {
	string name = "mkfs";
	string func = "��ʽ���ļ���";
	string format = "mkfs";
	string info = "��ʽ���󣬵�ǰĿ¼Ϊ/���û�Ϊroot";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
//Ŀ¼���
void mkdir_help() {
	string name = "mkdir";
	string func = "����Ŀ¼";
	string format = "mkdir fp (fpΪ·����)";
	string info = "Ljyux������Ŀ¼���ļ�����";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void rmdir_help() {
	string name = "rmdir";
	string func = "ɾ��Ŀ¼";
	string format = "rmdir fp(fpΪ·����)";
	string info = "ɾ��Ŀ¼ʱ���뱣֤Ŀ¼Ϊ��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void ls_help(){
	string name = "ls";
	string func = "չʾĿ¼��Ϣ";
	string format = "ls [-l] fp (fp��Ŀ¼·��)";
	string info = "fp�������������ļ�";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void cd_help(){
	string name = "cd";
	string func = "���ĵ�ǰĿ¼";
	string format = "cd fp(fpΪ·����)";
	string info = "��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
//�ļ����
void mkfile_help(){
	string name = "mkfile";
	string func = "�����ļ�";
	string format = "mkfile fp [-m auth](fpΪ�ļ�·��, authΪ�ļ�Ȩ�ޣ�16���Ʊ�ʾ) ";
	string info = "�������������ļ���������֧����������";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void rmfile_help(){
	string name = "rmfile";
	string func = "ɾ���ļ�";
	string format = "rmdir fp(fpΪ�ļ�·��)";
	string info = "ɾ���ļ�ʱ���뱣֤�ļ�������";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void fopen_help(){
	string name = "fopen";
	string func = "���ļ�";
	string format = "fopen fp [-rw|-ro](fpΪ�ļ�·��)";
	string info = "���ļ�ʱ��Ҫ��֤��Ȩ��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void fclose_help(){
	string name = "fclose";
	string func = "�ر��ļ�";
	string format = "fclose fp(fpΪ�ļ�·��)";
	string info = "���뱣֤�ļ��Ѵ�";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void fread_help(){
	string name = "fread";
	string func = "��ȡ�ļ�";
	string format = "fread fp [-n length](fpΪ�ļ�·����lengthΪ��ȡ����)";
	string info = "���뱣֤�ļ��򿪴Ӷ�дָ�뿪ʼ��ȡlength���ֽڵ���Ļ�У����޸Ķ�дָ��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void fwrite_help(){
	string name = "fwrite";
	string func = "д���ļ�";
	string format =
		"\nfwrite fp -s str [-n size] (fpΪ�ļ�·����strΪд�����ݣ�sizeΪд���С)\n"\
		"fwrite fp -i [-n size](����ģʽ��fpΪ�ļ�·����sizeΪд���С)";
	string info =
		"\n-s����ģʽ��֧���ַ������пո�ͻس��ȼ����"\
		"\n-iģʽ�£��Ե���#�ӻس���ʾ�������"\
		"\n���뱣֤�ļ��Ѵ�";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void fseek_help(){
	string name = "fseek";
	string func = "�ƶ��ļ���дָ��";
	string format =
		"\nfseek fp (fpΪ�ļ�·��, Ĭ�ϻص���ͷ)"\
		"\nfseek fp -b size (fpΪ�ļ�·�������ļ���ʼ����ƶ�size�ֽ�)"\
		"\nfseek fp -e size (fpΪ�ļ�·�������ļ���β��ǰ�ƶ�size�ֽ�)"\
		"\nfseek fp -a size (fpΪ�ļ�·�����ӵ�ǰλ���ƶ�size���ֽ�)";
	string info = "���뱣֤�ļ��Ѵ�";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void cat_help(){
	string name = "cat";
	string func = "����ļ�";
	string format = "cat fp(fpΪ�ļ�·��)";
	string info = "���뱣֤�ļ��Ѵ򿪣��˲������ı��дָ��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}

void load_help(){
	string name = "load";
	string func = "����������ȡ�ļ���Ljyux��";
	string format = "load src dst (srcΪ�������ļ�·����dstΪLjyux�ļ�·��)";
	string info = "���뱣֤�ļ��Ѵ򿪣��˲������ı��дָ�룬�Ḳ��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void dump_help(){
	string name = "dump";
	string func = "��Ljyuxд���ļ�����������";
	string format = "dump src dst (srcΪLjyux�ļ�·����dstΪ�������ļ�·��)";
	string info = "���뱣֤�ļ��Ѵ�";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}

//�û����
void whoami_help(){
	string name = "whoami";
	string func = "�鿴��ǰ�û�";
	string format = "whoami";
	string info = "��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void adduser_help(){
	string name = "adduser";
	string func = "����û�";
	string format = "adduser [-u name] [-p password] (nameΪ�û���,passwordΪ����)";
	string info = "�û��������������Ϊ13���������-u -p�����ں���Ҫ������";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void deluser_help(){
	string name = "deluser";
	string func = "ɾ���û�";
	string format = "deluser -u uname (unameΪ�û���)";
	string info = "���뱣֤�û����ڡ�ɾ��ʱ�������û����ļ�";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void su_help(){
	string name = "su";
	string func = "�����û�";
	string format = "su [- uname] (unameΪ�û���)";
	string info = "����ָ���û������л���root�û�";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void setgroup_help(){
	string name = "setgroup";
	string func = "Ϊ�û����÷���";
	string format = "setgroup -u uname -g gid (unameΪ�û�����gidΪid�ţ�Ҫ����0-15����������Ĭ��Ϊ0)";
	string info = "����ָ���û���gid";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void lsuser_help(){
	string name = "lsuser";
	string func = "չʾ�û�";
	string format = "lsuser";
	string info = "��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
//Ȩ�����
void chmod_help(){
	string name = "chmod";
	string func = "�����ļ�Ȩ��";
	string format = "chmod fp (u|g|v)(+|-)(r|w|x) (fpΪ�ļ�·��)";
	string info = "����Linux��chmod����";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void setmod_help(){
	string name = "setmod";
	string func = "�����ļ�Ȩ��";
	string format = "setmod fp mode (fpΪ�ļ�·����modeΪ�ļ�Ȩ�ޣ�16����)";
	string info = "Ȩ�����òο�Unix/Linux�ļ�Ȩ�ޣ������ļ�Ȩ�޲���Ϥ������ʹ�ô�����";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
//չʾ�̿�
void lsinode_help(){
	string name = "lsinode";
	string func = "չʾinode��Ϣ";
	string format = "lsinode inode_id (inode_id��inode��)";
	string info = "��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void lsblk_help(){
	string name = "lsblk";
	string func = "չʾ�̿�";
	string format = "lsblk blk_id (blk_id���̿��)";
	string info = "��";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}
void help_help() {
	string name = "help";
	string func = "չʾ������Ϣ";
	string format = "help [command] (commandΪ����)";
	string info = "����ָ������������������չʾ���п�������";

	cout << "��������: " << name << endl;
	cout << "�����ʽ: " << format << endl;
	cout << "��������: " << func << endl;
	cout << "����˵��: " << info << endl;
}

//����list
void helplist(){
	vector<string>commands{
		"mkfs","��ʽ���ļ���",
		"mkdir","����Ŀ¼",
		"rmdir","ɾ��Ŀ¼",
		"ls","չʾĿ¼��Ϣ",
		"cd","���ĵ�ǰĿ¼",
		"mkfile","�����ļ�",
		"rmfile","ɾ���ļ�",
		"fopen","���ļ�",
		"fclose","�ر��ļ�",
		"fread","��ȡ�ļ�",
		"fwrite","д���ļ�",
		"fseek","�ƶ��ļ���дָ��",
		"cat","����ļ�",
		"load","����������ȡ�ļ���Ljyux��",
		"dump","��Ljyuxд���ļ�����������" ,
		"whoami","�鿴��ǰ�û�",
		"adduser","����û�",
		"deluser","ɾ���û�",
		"su","�����û�",
		"setgroup","Ϊ�û����÷���",
		"lsinode","չʾinode��Ϣ",
		"lsblk","չʾ�̿�",
		"help","չʾ������Ϣ",
		"lsuser","չʾ�û�",
		"chmod","�����ļ�Ȩ��",
		"setmod","�����ļ�Ȩ��",

	};
	cout << "����ָ������: " << endl;
	for (u_int i = 0; i < commands.size() - 1; i += 2) {
		cout << commands[i] << " : " << commands[i + 1] << endl;
	}
}