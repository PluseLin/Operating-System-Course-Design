#include "Ljyux.h"

void Ljyux::initialize_DiskDevice() {
	g_Disk = new DiskDevice();
	m_Disk = g_Disk;

}

void Ljyux::destruct_DiskDevice() {

	delete g_Disk;
}

void Ljyux::initialize_global() {
	g_ftable = new OpenFileTable();
	g_itable = new InodeTable();
	g_utable = new UserTable();
	g_fs = new FileSystem();
	
	m_fs = g_fs;
	m_ftable = g_ftable;
	m_itable = g_itable;
	m_utable = g_utable;
}

void Ljyux::destruct_global() {
	//ȫ�ֱ���ָ���ͷ�
	delete g_ftable;
	delete g_itable; 
	delete g_utable;
	delete g_fs;
}

Ljyux::Ljyux() {
	//g_Diskһ��Ҫ�������룬��Ϊ�������������ʱ��
	//���ܻ����������ȥ����д
	initialize_DiskDevice();

	initialize_global();
	m_pfm = new FileManager;


	m_user = nullptr;
	m_curdir = "/";

	if (m_Disk->d_newfile) {
		cout << "�ļ������ڣ��Զ���ʽ��..." << endl;
		fformat();
	}
	
}

Ljyux::~Ljyux() {
	destruct_global();

	//g_Diskһ��Ҫ����ͷţ���Ϊ���������ͷŵ�ʱ��
	//���ܻ����������ȥ����д
	destruct_DiskDevice();
	delete m_pfm;
	//delete m_user;
}

bool Ljyux::is_root(User* user) {
	return user->m_uinfo->m_uid == 0;
}

bool Ljyux::has_authroity(Inode* inode, int umode) {
	int imode = inode->m_pinode->i_mode;
	int authority = 0;
	//�ж��ļ������ߺ͵�ǰ�û���ϵ
	User* holder = m_utable->UGet(inode->m_pinode->i_uid);
	//����Ҳ����û���������������û�������ʹ��
	if (holder == nullptr) {
		return true;
	}
	//ͬһ��
	if (holder->m_uinfo->m_uid == m_user->m_uinfo->m_uid) {
		//ȡ��ͬ���û���Ȩ��
		authority = (inode->m_pinode->i_mode & 0x700) >> 8;
	}
	//ͬһ��
	else if (holder->m_uinfo->m_gid == m_user->m_uinfo->m_gid) {
		authority = (inode->m_pinode->i_mode & 0x070) >> 4;
	}
	//����
	else {
		authority = (inode->m_pinode->i_mode & 0x007);
	}
	return (authority & umode) == umode;
}

string Ljyux::curdir() {
	return m_curdir;
}

bool Ljyux::login() {
	string pswd;
	string uname;
	cout << endl;
	cout << "�������û���: " ;
	char c = _getche();
	if (c == 27)
		exit(0);
	getline(cin, uname);
	uname = c + uname;
	cout << "����������: ";
	c = _getche();
	if (c == 27)
		exit(0);
	getline(cin, pswd);
	pswd = c + pswd;
	User* user = m_utable->UGet(uname);
	if (user == nullptr) {
		cout << "�û��������ڣ�" << endl;
		return false;
	}
	else if (user->m_uinfo->m_passwd != pswd) {
		cout << "���벻��ȷ��" << endl;
		return false;
	}
	//����m_user
	m_user = user;
	return true;
}

void Ljyux::fformat() {


	//�Ȱѳ���Disk����ľֲ�����ɾ��
	delete m_pfm;
	destruct_global();

	//�����´���
	initialize_global();
	//�ؽ�FileManager
	m_pfm = new FileManager;
	m_pfm->fformat(); 

	m_utable->reload();

	//�޸ĵ�ǰ��ַ
	m_curdir = "/";

	m_user = m_utable->UGet(0);
}

void Ljyux::mkdir(string fp) {
	//�ȿ����ܲ��ܴ���
	int status;
	Inode* inode = m_pfm->NameI(fp, status);
	if (status == DIR_EXIST) {
		cout << "Ŀ¼ " << fp << " �Ѵ��ڣ�" << endl;
	}
	else if (status == FATHER_NOT_EXIST) {
		cout << "·�� " << fp << " �����ڣ����飡" << endl;
	}
	else {
		string dir = get_filename(fp);
		m_pfm->AddDirEntry(inode, dir);
		inode->dump();
		cout << "Ŀ¼ " << fp << " �����ɹ���" << endl;
	}
	//inode->dump();
	g_itable->IPut(inode);
}

void Ljyux::rmdir(string fp) {
	//����1��Ŀ¼����
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DIRECTORY_FILE);
	if (status != DIR_EXIST) {
		cout << "Ŀ¼ " << fp << " �����ڣ�" << endl;
		g_itable->IPut(inode); return;
	}
	//����2��Ŀ¼�б���ֻ��.��..
	//����Ŀ¼û��..��������ɾ����
	vector<DirEntry> dirs = m_pfm->getDirs(inode);
	bool flag = true;
	int father_id = -1;
	for (auto& dir : dirs) {
		if (strcmp(dir.m_dname, ".") == 0) continue;
		else if (strcmp(dir.m_dname, "..") == 0) {
			father_id = dir.m_inode;
		}
		else
			flag = false;
	}
	if (!flag || father_id == -1) {
		cout << "���ȱ�֤Ŀ¼Ϊ�գ�" << endl;
		g_itable->IPut(inode); return;
	}
	//���ϼ�Ŀ¼
	Inode* fa_inode = fa_inode = g_itable->IGet(father_id);
	//�������Inode��Ӧ���̿�
	inode->FreeDisk();
	//�ϼ�Ŀ¼����
	m_pfm->RemoveDirEntry(fa_inode, inode->m_id);
	//�ر��ϼ�Ŀ¼����һ��Ŀ¼
	g_fs->IFree(inode->m_id);

	g_itable->IPut(inode);
	g_itable->IPut(fa_inode);


	cout << "ɾ��Ŀ¼ " << fp << " �ɹ���" << endl;
}

void Ljyux::ls(string fp) {
	//�ȿ���Ŀ¼�Ƿ����
	int status;
	Inode* inode = m_pfm->NameI(fp, status);
	if (status == DIR_EXIST) {
		m_pfm->ls_(inode);
		cout << endl;
	}
	else {
		cout << "·�� " << fp << " �����ڣ����飡" << endl;
	}
	if (inode->m_id != 0) {
		g_itable->IPut(inode);
	}
}

void Ljyux::ls_l(string fp) {
	//�ȿ���Ŀ¼�Ƿ����
	int status;
	Inode* inode = m_pfm->NameI(fp, status);
	if (status == DIR_EXIST) {
		m_pfm->ls_l(inode);
		cout << endl;
	}
	else {
		cout << "·�� " << fp << " �����ڣ����飡" << endl;
	}
	if (inode->m_id != 0) {
		g_itable->IPut(inode);
	}
}

void Ljyux::lsinode(int inode_id) {
	Inode* inode = g_itable->IGet(inode_id);
	cout << "inode_id : " << inode->m_id << endl;
	cout << "inode_size : " << inode->m_pinode->i_size / (inode->m_pinode->i_type == DIRECTORY_FILE ? 64 : 1) << endl;
	cout << "inode_mode : " << inode->authority() << endl;
	g_itable->IPut(inode);
}

void Ljyux::lsblk(int blk_id) {
	Block blk;
	if (blk_id >= DISK_BLOCK_NUM || blk_id < 0) {
		cout << "�̿�ŷǷ���" << endl;
		return;
	}
	g_Disk->read(blk_id, 0, &blk);
	//
	for (int _ = 0; _ < 60; _++) cout << "=";
	cout << endl;
	cout.setf(ios::uppercase);
	cout << "offset      ";
	for (int i = 0; i < 16; i++) {
		cout << hex<<setfill('0') << setw(2) << i << " ";
	}
	cout << endl;
	for (int i = 0; i < BLOCK_SIZE; i+=16) {
		cout << "0x";
		cout<< hex<< setfill('0') << setw(8)<< 64 * blk_id + i << "  ";
		for (int j = 0; j < 16; j++) {
			u_int byte = (u_char(blk.data[i + j]));
			cout << setfill('0') << setw(2) <<
			hex << byte << " ";
		}
		cout << endl;
	}
	cout << dec;
	for (int _ = 0; _ < 60; _++) cout << "=";
	cout << endl;
}

void Ljyux::cd(string fp) {
	if (fp == "")
		return;
	//���ж�·���Ƿ����
	int status;
	Inode* inode = m_pfm->NameI(fp, status);
	//�����ڣ��޸�curdir��FileManager�е�inode
	if (status == DIR_EXIST) {
		//�޸�curdir
		if (fp[0] != '/')
			m_curdir = abspath(join(m_curdir, fp));
		else
			m_curdir = abspath(fp);
		//�޸�FileManager��inode
		m_pfm->chinode(inode);
	}
	else {
		cout << "·�� " << fp << " �����ڻ�Ƿ������飡" << endl;
	}
	g_itable->IPut(inode);
}

//�ļ����
void Ljyux::fcreate(string fp,int imode) {
	//����1�����ж�·���Ƿ����
	//����Ŀ¼�����ڣ�������·���Ѵ��ڣ�����
	//��Ljyux�У��ݲ����������ļ���Ŀ¼�ļ�������
	int status = 0;
	Inode* inode = m_pfm->NameI(fp, status);
	if (status == FATHER_NOT_EXIST) {
		cout << "�ļ���Ŀ¼�����ڣ�" << endl;
		g_itable->IPut(inode); return;
	}
	else if (status == DIR_EXIST) {
		cout << "�ļ��Ѵ��ڣ�" << endl;
		g_itable->IPut(inode); return;
	}
	//����һ��Inode�����ڴ�����ͨ�ļ��������������FileManager
	//�޸��ļ�����Ϊ�����ļ���Ȩ�޳�ʼȫ��Ϊ1
	Inode* new_inode = m_pfm->fcreate(imode);
	//�޸������Ϣ
	new_inode->m_pinode->i_uid = m_user->m_uinfo->m_uid;
	new_inode->m_pinode->i_gid = m_user->m_uinfo->m_gid;
	//��Ŀ¼���һ��Ŀ¼��
	m_pfm->AddFile(inode, new_inode, get_filename(fp));
	//Ĭ�ϲ��򿪣�����Ҫ��������
	g_itable->IPut(new_inode);
	g_itable->IPut(inode);
	cout << fp << " �����ɹ���Ȩ��Ϊ" << hex << imode << dec << endl;
}

void Ljyux::fdelete(string fp) {
	//����1�����ж�·���Ƿ���ڣ���Ϊ�����ļ�
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "�ļ������ڣ�" << endl;
		g_itable->IPut(inode); return;
	}
	//����2���û����ж�ȡд��Ȩ��
	//���û��£��ݲ�����
	if (!has_authroity(inode, UREAD | UWRITE)) {
		cout << "�޶�ȡȨ��" << endl;
		g_itable->IPut(inode); return;
	}
	//����3��û���û���֮
	if (inode->m_count != 0) {
		cout << "�ļ��ѱ��򿪣����ȹر�֮" << endl;
		return;
	}
	//���Ϻ󣬷���ɾ��
	//�跨ȡ�����ڵ�
	string father_fp = get_father(abspath(join(m_curdir, fp)));
	Inode* fa_inode = m_pfm->NameI(get_father(fp), status);
	if (status != DIR_EXIST) {
		cout << "��Ŀ¼�����ڣ�" << endl;
		g_itable->IPut(inode); 
		g_itable->IPut(fa_inode);
		return;
	}
	//���մ��ļ�inode���е��̿�
	inode->FreeDisk();
	//�Ӹ��ڵ����
	m_pfm->RemoveDirEntry(fa_inode, inode->m_id);
	//����inode

	g_fs->IFree(inode->m_id);

	g_itable->IPut(fa_inode);
	g_itable->IPut(inode);

	cout << "ɾ���ļ� " << fp << " �ɹ���" << endl;
}

void Ljyux::fopen(string fp, int fmode) {
	//����1:���ж�·���Ƿ���ڣ���Ϊ�����ļ�
	int status = 0;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "·�������ڣ�" << endl;
		g_itable->IPut(inode); return;
	}
	else if (inode->m_pinode->i_type == DIRECTORY_FILE) {
		cout << "Ŀ¼�ļ����ɴ򿪣�" << endl;
		g_itable->IPut(inode); return;
	}
	else if(m_user->getfd(inode->m_id)!=-1){
		cout << "���ļ��Ѿ��򿪣�" << endl;
		g_itable->IPut(inode); return;
	}
	else {
		//��������1������2:�ж��Ƿ��д�Ȩ��
		//�����û��£��ݲ����ǣ�
		int umode = 0;
		umode |= (fmode & OpenFileMode::FWRITE)  ? UWRITE : 0;
		umode |= (fmode & OpenFileMode::FREAD)  ? UREAD : 0;
		if (!has_authroity(inode,umode)/*���޸�*/) {
			cout << "�޴�Ȩ�ޣ�" << endl;
			g_itable->IPut(inode); return;
		}
		else {
			//�����������û��д򿪣�����inode
			Inode* r_inode = m_ftable->Ifind(inode->m_id);
			if (r_inode != nullptr) {
				g_itable->IPut(inode); inode = r_inode;
			}
			inode->m_count++;
			//��������1,2������File�ṹ��������֮
			File* file = m_ftable->Falloc();
			if (file == nullptr) {
				cout << "���ļ���Ŀ�ﵽ���ֵ��" << endl;
				g_itable->IPut(inode); return;
			}
			file->setF(inode, fmode);
			//��ӵ�User�ṹ��OpenFile��
			int fd = m_user->m_files->AllocFd();
			if (fd == -1) {
				cout << "�û����ļ���Ŀ�ﵽ���ޣ�" << endl;
				g_itable->IPut(inode);
				return;
			}
			m_user->m_files->SetF(fd, file);
			//��User�У������ļ������inode�Ĺ���
			m_user->setfd(fd, file->m_inode_id);
		}
		cout << fp << " �򿪳ɹ�����ģʽΪ:";
		cout << ((fmode & OpenFileMode::FWRITE) != 0 ? "rw" : "ro") << endl;
	}
}

void Ljyux::fclose(string fp) {
	//����1��·�����ڣ���Ϊ�����ļ�
	int status = 0;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "·�������ڣ�" << endl;
		g_itable->IPut(inode); return;
	}
	else if (inode->m_pinode->i_type == DIRECTORY_FILE) {
		cout << "Ŀ¼�ļ����ɹرգ�" << endl;
		g_itable->IPut(inode); return;
	}
	//����2���ļ��Ѿ���
	int fd = m_user->getfd(inode->m_id);
	if (fd == -1) {
		cout << "�ļ���δ�򿪣�" << endl;
		return;
	}
	g_itable->IPut(inode);
	File* file = m_user->m_files->GetF(fd);
	inode = file->m_pinode;
	//����ǰ�������������ɹر��ļ�
	//inode���ü����ݼ�������Ϊ0����
	inode->m_count--;
	int inode_id = inode->m_id;
	if (inode->m_count == 0) {
		g_itable->IPut(inode);
	}
	//File�ṹ����,OpenFilesȥ����һ��
	m_ftable->Ffree(file);

	m_user->m_files->FreeFd(fd);
	m_user->freefd(inode_id);

	//������ϲ��裬����ɹ�
	cout << "�ļ� " << fp << "�رճɹ���" << endl;
}

void Ljyux::fread(string fp, int rsize,string& str) {
	//����1����֤·��������Ϊ�����ļ�
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "�ļ�������!" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//ֻ��Ҫinode_id�����ȷ�Ϻ��ͷ�
	g_itable->IPut(inode);
	//����2���ļ��Ѵ򿪣����ж�Ȩ��
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FREAD) == 0) {
		cout << "�ļ��޶�Ȩ�ޣ�" << endl;
		return;
	}
	//֮��ȷ�϶�ȡ��С��ʵ�ֶ�ȡ��������file��f_offset
	rsize = min(file->m_pinode->m_pinode->i_size - file->m_foffset, rsize);
	char* dst = new char[rsize + 1];
	rsize=file->m_pinode->ReadI(dst, file->m_foffset, rsize);
	file->m_foffset += rsize;
	dst[rsize] = 0;
	//���
	str = dst;
	delete[] dst;
}

void Ljyux::fwrite(string fp, string src, int wsize) {
	//����1����֤·��������Ϊ�����ļ�
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "�ļ�������!" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//ֻ��Ҫinode_id�����ȷ�Ϻ��ͷ�
	g_itable->IPut(inode);
	//����2���ļ��Ѵ򿪣�����дȨ��
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FWRITE) == 0) {
		cout << "�ļ���дȨ�ޣ�" << endl;
		return;
	}
	//֮��ȷ��д�룬ʵ�ֶ�ȡ��������file��f_offset
	wsize = min(wsize, int(src.length()));
	char* dst = new char[wsize + 1];
	memcpy(dst, src.c_str(), wsize + 1);
	//strcpy(dst, src.c_str());
	wsize=file->m_pinode->WriteI(dst, file->m_foffset, wsize);
	file->m_foffset += wsize;
	cout << "�ɹ�д��" << wsize << "���ֽ�" << endl;
	delete[]dst;
}

void Ljyux::fseek(string fp, int mode, int offset) {
	//����1����֤·��������Ϊ�����ļ�
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "�ļ�������!" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//ֻ��Ҫinode_id�����ȷ�Ϻ��ͷ�
	g_itable->IPut(inode);
	//����2���ļ��Ѵ�
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	//�޸�m_foffset
	//��ͷ����
	int fsize = file->m_pinode->m_pinode->i_size;
	if (mode == FSEEK_BEGIN) {
		file->m_foffset = min(offset, fsize);
	}
	//��β����
	else if (mode == FSEEK_END) {
		file->m_foffset = fsize - min(offset, fsize);
	}
	//׷��
	else if (mode == FSEEK_ADD) {
		file->m_foffset += min(offset, fsize - file->m_foffset);
	}
	cout << "��ǰ�ļ�ָ��λ��: " << file->m_foffset << endl;
}

void Ljyux::cat(string fp) {
	//����1��fp����
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "Ŀ���ļ������ڣ�" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//ֻ��Ҫinode_id�����ȷ�Ϻ��ͷ�
	g_itable->IPut(inode);
	//����2��fp�Ѵ��ҿɶ�
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FREAD) == 0) {
		cout << "�ļ���дȨ�ޣ�" << endl;
		return;
	}
	//֮��ÿ������ȡ512�ֽڣ����
	char buffer[BLOCK_SIZE + 1] = { 0 };
	int rsize = 0;
	while (rsize < file->m_pinode->m_pinode->i_size) {
		//��
		int temp = file->m_pinode->ReadI(buffer, rsize, BLOCK_SIZE);
		rsize += temp;
		//���
		cout << buffer << endl;
	}
}

void Ljyux::load(string src, string dst) {
	//����1��src����
	ifstream fin(src, ios::binary | ios::in);
	if (!fin.is_open()) {
		cout << "Դ�ļ������ڻ��޷��򿪣�" << endl;
		return;
	}
	//����2��dst����
	int status;
	Inode* inode = m_pfm->NameI(dst, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "Ŀ���ļ������ڣ�" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//ֻ��Ҫinode_id�����ȷ�Ϻ��ͷ�
	g_itable->IPut(inode);
	//����3��Ŀ���ļ��Ѵ�
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FWRITE) == 0) {
		cout << "�ļ���дȨ�ޣ�" << endl;
		return;
	}
	//����ļ�����
	Block blk;
	fin.seekg(0, ios::beg);
	int offset = 0;
	file->m_pinode->m_pinode->i_size = 0;
	while (!fin.eof()) {
		//��
		fin.read(blk.data, BLOCK_SIZE);
		//д
		int wsize = file->m_pinode->WriteI
		(blk.data, offset, fin.gcount());
		offset += wsize;
	}
	cout << "�ɹ�д�� " << offset << " ���ֽ�" << endl;
	//this->lsblk(4);
	fin.close();
}

void Ljyux::dump(string src, string dst) {
	//����1��dst��д��
	ofstream fout(dst, ios::binary | ios::out);
	if (!fout.is_open()) {
		cout << "Ŀ���ļ�����д��" << endl;
		return;
	}
	//����2��src����
	int status;
	Inode* inode = m_pfm->NameI(src, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "Ŀ���ļ������ڣ�" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//ֻ��Ҫinode_id�����ȷ�Ϻ��ͷ�
	g_itable->IPut(inode);
	//����3��src�Ѵ򿪣����ж�Ȩ��
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "�ļ�δ�򿪣�" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FREAD) == 0) {
		cout << "�ļ���дȨ�ޣ�" << endl;
		return;
	}
	//�ļ�ȫ������
	int fsize = file->m_pinode->m_pinode->i_size;
	int offset = 0;
	Block blk;
	while (offset < fsize) {
		//��
		int rsize = file->m_pinode->ReadI(blk.data, offset, BLOCK_SIZE);
		offset += rsize;
		//д
		fout.write(blk.data, rsize);
	}

	cout << "д���� " << fsize << " ���ֽڣ�" << endl;
	fout.close();
}

void Ljyux::chmod(string src, int mode, bool is_add) {
	//����1:·������
	int status;
	Inode* inode = m_pfm->NameI(src, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "Ŀ���ļ������ڣ�" << endl;
		m_itable->IPut(inode); return;
	}
	//����2:�������Լ����ļ�
	if (inode->m_pinode->i_uid != inode->m_pinode->i_uid) {
		cout << "���ǵ�ǰ�û����ļ����������޸�Ȩ�ޣ�" << endl;
		m_itable->IPut(inode); return;
	}
	//����3 �ļ�δ����
	if (inode->m_count != 0) {
		cout << "�ļ��ѱ��򿪣����ȹر��ļ���" << endl;
		m_itable->IPut(inode); return;
	}
	//ֱ�Ӹ�
	if (is_add) {
		inode->m_pinode->i_mode |= mode;
	}
	else {
		inode->m_pinode->i_mode &= (~mode);
	}
	cout << "�ļ�Ȩ���޸ĳɹ���" << endl;

	m_itable->IPut(inode);
}

void Ljyux::setmod(string src, int mode) {
	//����1:·������
	int status;
	Inode* inode = m_pfm->NameI(src, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "Ŀ���ļ������ڣ�" << endl;
		m_itable->IPut(inode); return;
	}
	//����2:�������Լ����ļ�
	if (inode->m_pinode->i_uid != inode->m_pinode->i_uid) {
		cout << "���ǵ�ǰ�û����ļ�������������Ȩ�ޣ�" << endl;
		m_itable->IPut(inode); return;
	}
	//����3 �ļ�δ����
	if (inode->m_count != 0) {
		cout << "�ļ��ѱ��򿪣����ȹر��ļ���" << endl;
		m_itable->IPut(inode); return;
	}
	//ֱ�Ӹ�
	inode->m_pinode->i_mode = mode;
	cout << "�ļ�Ȩ���޸ĳɹ���" << endl;

	m_itable->IPut(inode);
}

void Ljyux::whoami() {
	//����û���Ϣ����
	if (m_user == nullptr)
		return;
	cout << "��ǰ�û� : " << m_user->m_uinfo->m_name << endl;
	cout << "�û�uid : " << m_user->m_uinfo->m_uid << endl;
	cout << "�û�gid : " << m_user->m_uinfo->m_gid << endl;
}

void Ljyux::adduser(string uname, string pswd) {
	//���ж��Ƿ��ܼ���
	//����1:��ǰ�û�������root�û�
	if (!is_root(m_user)) {
		cout << "ֻ��root�û����д�Ȩ�ޣ�" << endl;
		return;
	}
	//����2:�û�������
	if (m_utable->UGet(uname) != nullptr) {
		cout << "�û��Ѵ��ڣ�" << endl;
		return;
	}
	//����3:�û���δ��
	User* user = m_utable->UAlloc();
	if (user == nullptr) {
		cout << "�û���Ŀ�Ѵﵽϵͳ���ޣ�" << endl;
		return;
	}
	//����󣬿ɼ��룬����һЩ����
	strcpy(user->m_uinfo->m_name, uname.c_str());
	strcpy(user->m_uinfo->m_passwd, pswd.c_str());
	
	cout << "����û�" << uname << "�ɹ���";
	cout << "uid: " << user->m_uinfo->m_uid << " ";
	cout << "gid: " << user->m_uinfo->m_gid << endl;
}

void Ljyux::deluser(string uname) {
	//���ж��Ƿ���ɾ��
	//����1:��ǰ�û�������root�û�
	if (!is_root(m_user)) {
		cout << "ֻ��root�û����д�Ȩ�ޣ�" << endl;
		return;
	}
	//����2:��ɾ���û������Ҳ�����root�û�
	User* user = m_utable->UGet(uname);
	if (user == nullptr) {
		cout << "�û������ڣ�" << endl;
		return;
	}
	else if (is_root(user)) {
		cout << "������ɾ��root�û���" << endl;
		return;
	}
	//����󣬿���ɾ��
	m_utable->UDelete(user);

	cout << "ɾ���û�" << uname << "�ɹ���" << endl;
}

void Ljyux::su(string uname) {
	//���ж��û��Ƿ����
	User* user = m_utable->UGet(uname);
	if (user == nullptr) {
		cout << "�û������ڣ�" << endl;
		return;
	}
	//�����ڣ���������
	cout << "���������� : ";
	string pswd; cin >> pswd;
	if (pswd != user->m_uinfo->m_passwd) {
		cout << "�������" << endl;
		return;
	}
	while (getchar() != '\n')continue;
	m_user = user;
	cout << "�޸��û��ɹ�����ǰ�û�Ϊ: " << uname << endl;
}

void Ljyux::setgroup(string uname, int gid) {
	//���ж��Ƿ����޸�gid
	//����1:��ǰ�û�������root�û�
	if (!is_root(m_user)) {
		cout << "ֻ��root�û����д�Ȩ�ޣ�" << endl;
		return;
	}
	//����2:�û�����
	User* user = m_utable->UGet(uname);
	if (user == nullptr) {
		cout << "�û�������" << endl;
		return;
	}
	user->m_uinfo->m_gid = gid;
	cout << "�޸�group id�ɹ���" << endl;
}

void Ljyux::lsuser() {
	//��m_utable��ȡ�������û�
	vector<User*> users = m_utable->GetAllUser();
	vector<string> titles{
		"user","uid","gid"
	};
	for (auto& title : titles) {
		cout << title << "\t ";
	}
	cout << endl;
	for (User* u : users) {
		//username	 u_id	 g_id
		//root	0	0
		cout << u->m_uinfo->m_name << "\t ";
		cout << u->m_uinfo->m_uid << "\t ";
		cout << u->m_uinfo->m_gid << endl;
	}
}