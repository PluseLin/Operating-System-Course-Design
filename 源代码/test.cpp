#include"Ljyux.h"



void test1() {
	Ljyux os;
	//���Զ�д
	FileSystem* fs = g_fs;
	g_Disk;
	fs->fformat();
	int i = fs->Ialloc();
	Inode inode(i);
	char* buf = new char[1000];
	memset(buf, 'a', 1000);
	int size = inode.WriteI(buf, 0, 1000);
	cout << "д����" << size << "���ֽ�" << endl;
	char* buf1 = new char[500];
	memset(buf1, 0, 500);
	int size1 = inode.ReadI(buf1, 900, 500);
	cout << "��ȡ����" << size1 << "���ֽ�" << endl;
	cout << strlen(buf1) << endl;
	cout << buf1 << endl;
	fs->IFree(i);
}

void test2() {
	Ljyux os;
	if (0) {
		//���Ը�ʽ����·��
		os.m_pfm->fformat();
		int st;
		vector<string> fps{ "/./././bin","/bin","/bin/src" };
		for (auto& str : fps) {
			os.m_pfm->NameI(str, st);
			if (st == SearchDirStatus::DIR_EXIST) {
				cout << str << " Ŀ¼���ڣ�" << endl;
			}
			else if (st == SearchDirStatus::CURDIR_NOT_EXIST) {
				cout << str << " Ŀ¼�����ڣ���Ŀ¼���ڣ�" << endl;
			}
			else {
				cout << str << " ��Ŀ¼�����ڣ�" << endl;
			}
		}
	}
	if (0) {
		os.fformat();
		os.ls("/");
	}
	if (1) {
		os.fformat();
		//�����½�Ŀ¼
		vector<string> vec{
			"/bin","/etc",
			"/home",
			"/dev",
			"/home/texts","/home/reports","/home/photos",
			"/usr/Jerry"
		};
		for (auto& fp : vec) {
			os.mkdir(fp);
		}
		os.ls("/");
		os.ls("/home");
		os.ls("/bin");
		os.ls("/usr");
	}
}

void test3() {

}

