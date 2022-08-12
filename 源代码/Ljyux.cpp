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
	//全局变量指针释放
	delete g_ftable;
	delete g_itable; 
	delete g_utable;
	delete g_fs;
}

Ljyux::Ljyux() {
	//g_Disk一定要最先申请，因为其他类在申请的时候
	//可能会引用这个类去做读写
	initialize_DiskDevice();

	initialize_global();
	m_pfm = new FileManager;


	m_user = nullptr;
	m_curdir = "/";

	if (m_Disk->d_newfile) {
		cout << "文件卷不存在，自动格式化..." << endl;
		fformat();
	}
	
}

Ljyux::~Ljyux() {
	destruct_global();

	//g_Disk一定要最后释放，因为其他类在释放的时候
	//可能会引用这个类去做读写
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
	//判断文件持有者和当前用户关系
	User* holder = m_utable->UGet(inode->m_pinode->i_uid);
	//如果找不到用户，大概率是无主用户，可以使用
	if (holder == nullptr) {
		return true;
	}
	//同一人
	if (holder->m_uinfo->m_uid == m_user->m_uinfo->m_uid) {
		//取出同组用户的权限
		authority = (inode->m_pinode->i_mode & 0x700) >> 8;
	}
	//同一组
	else if (holder->m_uinfo->m_gid == m_user->m_uinfo->m_gid) {
		authority = (inode->m_pinode->i_mode & 0x070) >> 4;
	}
	//其他
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
	cout << "请输入用户名: " ;
	char c = _getche();
	if (c == 27)
		exit(0);
	getline(cin, uname);
	uname = c + uname;
	cout << "请输入密码: ";
	c = _getche();
	if (c == 27)
		exit(0);
	getline(cin, pswd);
	pswd = c + pswd;
	User* user = m_utable->UGet(uname);
	if (user == nullptr) {
		cout << "用户名不存在！" << endl;
		return false;
	}
	else if (user->m_uinfo->m_passwd != pswd) {
		cout << "密码不正确！" << endl;
		return false;
	}
	//更改m_user
	m_user = user;
	return true;
}

void Ljyux::fformat() {


	//先把除了Disk以外的局部变量删了
	delete m_pfm;
	destruct_global();

	//再重新创建
	initialize_global();
	//重建FileManager
	m_pfm = new FileManager;
	m_pfm->fformat(); 

	m_utable->reload();

	//修改当前地址
	m_curdir = "/";

	m_user = m_utable->UGet(0);
}

void Ljyux::mkdir(string fp) {
	//先看看能不能创建
	int status;
	Inode* inode = m_pfm->NameI(fp, status);
	if (status == DIR_EXIST) {
		cout << "目录 " << fp << " 已存在！" << endl;
	}
	else if (status == FATHER_NOT_EXIST) {
		cout << "路径 " << fp << " 不存在，请检查！" << endl;
	}
	else {
		string dir = get_filename(fp);
		m_pfm->AddDirEntry(inode, dir);
		inode->dump();
		cout << "目录 " << fp << " 创建成功！" << endl;
	}
	//inode->dump();
	g_itable->IPut(inode);
}

void Ljyux::rmdir(string fp) {
	//条件1，目录存在
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DIRECTORY_FILE);
	if (status != DIR_EXIST) {
		cout << "目录 " << fp << " 不存在！" << endl;
		g_itable->IPut(inode); return;
	}
	//条件2，目录中必须只有.和..
	//（根目录没有..，不允许删除）
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
		cout << "请先保证目录为空！" << endl;
		g_itable->IPut(inode); return;
	}
	//打开上级目录
	Inode* fa_inode = fa_inode = g_itable->IGet(father_id);
	//回收这个Inode对应的盘块
	inode->FreeDisk();
	//上级目录除名
	m_pfm->RemoveDirEntry(fa_inode, inode->m_id);
	//关闭上级目录和这一级目录
	g_fs->IFree(inode->m_id);

	g_itable->IPut(inode);
	g_itable->IPut(fa_inode);


	cout << "删除目录 " << fp << " 成功！" << endl;
}

void Ljyux::ls(string fp) {
	//先看看目录是否存在
	int status;
	Inode* inode = m_pfm->NameI(fp, status);
	if (status == DIR_EXIST) {
		m_pfm->ls_(inode);
		cout << endl;
	}
	else {
		cout << "路径 " << fp << " 不存在，请检查！" << endl;
	}
	if (inode->m_id != 0) {
		g_itable->IPut(inode);
	}
}

void Ljyux::ls_l(string fp) {
	//先看看目录是否存在
	int status;
	Inode* inode = m_pfm->NameI(fp, status);
	if (status == DIR_EXIST) {
		m_pfm->ls_l(inode);
		cout << endl;
	}
	else {
		cout << "路径 " << fp << " 不存在，请检查！" << endl;
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
		cout << "盘块号非法！" << endl;
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
	//先判断路径是否存在
	int status;
	Inode* inode = m_pfm->NameI(fp, status);
	//若存在，修改curdir和FileManager中的inode
	if (status == DIR_EXIST) {
		//修改curdir
		if (fp[0] != '/')
			m_curdir = abspath(join(m_curdir, fp));
		else
			m_curdir = abspath(fp);
		//修改FileManager的inode
		m_pfm->chinode(inode);
	}
	else {
		cout << "路径 " << fp << " 不存在或非法，请检查！" << endl;
	}
	g_itable->IPut(inode);
}

//文件相关
void Ljyux::fcreate(string fp,int imode) {
	//条件1：先判断路径是否存在
	//若父目录不存在，报错；若路径已存在，报错
	//（Ljyux中，暂不允许数据文件和目录文件重名）
	int status = 0;
	Inode* inode = m_pfm->NameI(fp, status);
	if (status == FATHER_NOT_EXIST) {
		cout << "文件父目录不存在！" << endl;
		g_itable->IPut(inode); return;
	}
	else if (status == DIR_EXIST) {
		cout << "文件已存在！" << endl;
		g_itable->IPut(inode); return;
	}
	//申请一个Inode，用于创建普通文件，这项工作交付给FileManager
	//修改文件类型为数据文件，权限初始全设为1
	Inode* new_inode = m_pfm->fcreate(imode);
	//修改相关信息
	new_inode->m_pinode->i_uid = m_user->m_uinfo->m_uid;
	new_inode->m_pinode->i_gid = m_user->m_uinfo->m_gid;
	//父目录添加一个目录项
	m_pfm->AddFile(inode, new_inode, get_filename(fp));
	//默认不打开，不需要后续操作
	g_itable->IPut(new_inode);
	g_itable->IPut(inode);
	cout << fp << " 创建成功！权限为" << hex << imode << dec << endl;
}

void Ljyux::fdelete(string fp) {
	//条件1：先判断路径是否存在，且为数据文件
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "文件不存在！" << endl;
		g_itable->IPut(inode); return;
	}
	//条件2：用户具有读取写入权限
	//单用户下，暂不考虑
	if (!has_authroity(inode, UREAD | UWRITE)) {
		cout << "无读取权限" << endl;
		g_itable->IPut(inode); return;
	}
	//条件3：没有用户打开之
	if (inode->m_count != 0) {
		cout << "文件已被打开！清先关闭之" << endl;
		return;
	}
	//符合后，方可删除
	//设法取出父节点
	string father_fp = get_father(abspath(join(m_curdir, fp)));
	Inode* fa_inode = m_pfm->NameI(get_father(fp), status);
	if (status != DIR_EXIST) {
		cout << "父目录不存在！" << endl;
		g_itable->IPut(inode); 
		g_itable->IPut(fa_inode);
		return;
	}
	//回收此文件inode持有的盘块
	inode->FreeDisk();
	//从父节点除名
	m_pfm->RemoveDirEntry(fa_inode, inode->m_id);
	//回收inode

	g_fs->IFree(inode->m_id);

	g_itable->IPut(fa_inode);
	g_itable->IPut(inode);

	cout << "删除文件 " << fp << " 成功！" << endl;
}

void Ljyux::fopen(string fp, int fmode) {
	//条件1:先判断路径是否存在，且为数据文件
	int status = 0;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "路径不存在！" << endl;
		g_itable->IPut(inode); return;
	}
	else if (inode->m_pinode->i_type == DIRECTORY_FILE) {
		cout << "目录文件不可打开！" << endl;
		g_itable->IPut(inode); return;
	}
	else if(m_user->getfd(inode->m_id)!=-1){
		cout << "该文件已经打开！" << endl;
		g_itable->IPut(inode); return;
	}
	else {
		//符合条件1，条件2:判断是否有打开权限
		//（单用户下，暂不考虑）
		int umode = 0;
		umode |= (fmode & OpenFileMode::FWRITE)  ? UWRITE : 0;
		umode |= (fmode & OpenFileMode::FREAD)  ? UREAD : 0;
		if (!has_authroity(inode,umode)/*待修改*/) {
			cout << "无打开权限！" << endl;
			g_itable->IPut(inode); return;
		}
		else {
			//若有在其他用户中打开，更换inode
			Inode* r_inode = m_ftable->Ifind(inode->m_id);
			if (r_inode != nullptr) {
				g_itable->IPut(inode); inode = r_inode;
			}
			inode->m_count++;
			//符合条件1,2，申请File结构，并构建之
			File* file = m_ftable->Falloc();
			if (file == nullptr) {
				cout << "打开文件数目达到最大值！" << endl;
				g_itable->IPut(inode); return;
			}
			file->setF(inode, fmode);
			//添加到User结构的OpenFile中
			int fd = m_user->m_files->AllocFd();
			if (fd == -1) {
				cout << "用户打开文件数目达到上限！" << endl;
				g_itable->IPut(inode);
				return;
			}
			m_user->m_files->SetF(fd, file);
			//在User中，做好文件句柄与inode的勾连
			m_user->setfd(fd, file->m_inode_id);
		}
		cout << fp << " 打开成功，打开模式为:";
		cout << ((fmode & OpenFileMode::FWRITE) != 0 ? "rw" : "ro") << endl;
	}
}

void Ljyux::fclose(string fp) {
	//条件1：路径存在，且为数据文件
	int status = 0;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "路径不存在！" << endl;
		g_itable->IPut(inode); return;
	}
	else if (inode->m_pinode->i_type == DIRECTORY_FILE) {
		cout << "目录文件不可关闭！" << endl;
		g_itable->IPut(inode); return;
	}
	//条件2：文件已经打开
	int fd = m_user->getfd(inode->m_id);
	if (fd == -1) {
		cout << "文件尚未打开！" << endl;
		return;
	}
	g_itable->IPut(inode);
	File* file = m_user->m_files->GetF(fd);
	inode = file->m_pinode;
	//满足前两个条件，即可关闭文件
	//inode引用计数递减，若减为0回收
	inode->m_count--;
	int inode_id = inode->m_id;
	if (inode->m_count == 0) {
		g_itable->IPut(inode);
	}
	//File结构回收,OpenFiles去除这一项
	m_ftable->Ffree(file);

	m_user->m_files->FreeFd(fd);
	m_user->freefd(inode_id);

	//完成如上步骤，才算成功
	cout << "文件 " << fp << "关闭成功！" << endl;
}

void Ljyux::fread(string fp, int rsize,string& str) {
	//条件1：保证路径存在且为数据文件
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "文件不存在!" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//只需要inode_id，因此确认后即释放
	g_itable->IPut(inode);
	//条件2：文件已打开，且有读权限
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "文件未打开！" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "文件未打开！" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FREAD) == 0) {
		cout << "文件无读权限！" << endl;
		return;
	}
	//之后，确认读取大小，实现读取，并调整file的f_offset
	rsize = min(file->m_pinode->m_pinode->i_size - file->m_foffset, rsize);
	char* dst = new char[rsize + 1];
	rsize=file->m_pinode->ReadI(dst, file->m_foffset, rsize);
	file->m_foffset += rsize;
	dst[rsize] = 0;
	//输出
	str = dst;
	delete[] dst;
}

void Ljyux::fwrite(string fp, string src, int wsize) {
	//条件1：保证路径存在且为数据文件
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "文件不存在!" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//只需要inode_id，因此确认后即释放
	g_itable->IPut(inode);
	//条件2：文件已打开，且有写权限
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "文件未打开！" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "文件未打开！" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FWRITE) == 0) {
		cout << "文件无写权限！" << endl;
		return;
	}
	//之后，确认写入，实现读取，并调整file的f_offset
	wsize = min(wsize, int(src.length()));
	char* dst = new char[wsize + 1];
	memcpy(dst, src.c_str(), wsize + 1);
	//strcpy(dst, src.c_str());
	wsize=file->m_pinode->WriteI(dst, file->m_foffset, wsize);
	file->m_foffset += wsize;
	cout << "成功写入" << wsize << "个字节" << endl;
	delete[]dst;
}

void Ljyux::fseek(string fp, int mode, int offset) {
	//条件1：保证路径存在且为数据文件
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "文件不存在!" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//只需要inode_id，因此确认后即释放
	g_itable->IPut(inode);
	//条件2：文件已打开
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "文件未打开！" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "文件未打开！" << endl;
		return;
	}
	//修改m_foffset
	//从头遍历
	int fsize = file->m_pinode->m_pinode->i_size;
	if (mode == FSEEK_BEGIN) {
		file->m_foffset = min(offset, fsize);
	}
	//从尾遍历
	else if (mode == FSEEK_END) {
		file->m_foffset = fsize - min(offset, fsize);
	}
	//追加
	else if (mode == FSEEK_ADD) {
		file->m_foffset += min(offset, fsize - file->m_foffset);
	}
	cout << "当前文件指针位置: " << file->m_foffset << endl;
}

void Ljyux::cat(string fp) {
	//条件1：fp存在
	int status;
	Inode* inode = m_pfm->NameI(fp, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "目标文件不存在！" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//只需要inode_id，因此确认后即释放
	g_itable->IPut(inode);
	//条件2：fp已打开且可读
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "文件未打开！" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "文件未打开！" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FREAD) == 0) {
		cout << "文件无写权限！" << endl;
		return;
	}
	//之后，每次最多读取512字节，输出
	char buffer[BLOCK_SIZE + 1] = { 0 };
	int rsize = 0;
	while (rsize < file->m_pinode->m_pinode->i_size) {
		//读
		int temp = file->m_pinode->ReadI(buffer, rsize, BLOCK_SIZE);
		rsize += temp;
		//输出
		cout << buffer << endl;
	}
}

void Ljyux::load(string src, string dst) {
	//条件1：src存在
	ifstream fin(src, ios::binary | ios::in);
	if (!fin.is_open()) {
		cout << "源文件不存在或无法打开！" << endl;
		return;
	}
	//条件2：dst存在
	int status;
	Inode* inode = m_pfm->NameI(dst, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "目标文件不存在！" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//只需要inode_id，因此确认后即释放
	g_itable->IPut(inode);
	//条件3：目标文件已打开
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "文件未打开！" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "文件未打开！" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FWRITE) == 0) {
		cout << "文件无写权限！" << endl;
		return;
	}
	//完成文件传输
	Block blk;
	fin.seekg(0, ios::beg);
	int offset = 0;
	file->m_pinode->m_pinode->i_size = 0;
	while (!fin.eof()) {
		//读
		fin.read(blk.data, BLOCK_SIZE);
		//写
		int wsize = file->m_pinode->WriteI
		(blk.data, offset, fin.gcount());
		offset += wsize;
	}
	cout << "成功写入 " << offset << " 个字节" << endl;
	//this->lsblk(4);
	fin.close();
}

void Ljyux::dump(string src, string dst) {
	//条件1：dst可写入
	ofstream fout(dst, ios::binary | ios::out);
	if (!fout.is_open()) {
		cout << "目标文件不可写入" << endl;
		return;
	}
	//条件2：src存在
	int status;
	Inode* inode = m_pfm->NameI(src, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "目标文件不存在！" << endl;
		return;
	}
	int inode_id = inode->m_id;
	//只需要inode_id，因此确认后即释放
	g_itable->IPut(inode);
	//条件3：src已打开，且有读权限
	int fd = m_user->getfd(inode_id);
	if (fd == -1) {
		cout << "文件未打开！" << endl;
		return;
	}
	File* file = m_user->m_files->GetF(fd);
	if (file == nullptr) {
		cout << "文件未打开！" << endl;
		return;
	}
	if ((file->m_fmode & OpenFileMode::FREAD) == 0) {
		cout << "文件无写权限！" << endl;
		return;
	}
	//文件全部读出
	int fsize = file->m_pinode->m_pinode->i_size;
	int offset = 0;
	Block blk;
	while (offset < fsize) {
		//读
		int rsize = file->m_pinode->ReadI(blk.data, offset, BLOCK_SIZE);
		offset += rsize;
		//写
		fout.write(blk.data, rsize);
	}

	cout << "写出了 " << fsize << " 个字节！" << endl;
	fout.close();
}

void Ljyux::chmod(string src, int mode, bool is_add) {
	//条件1:路径存在
	int status;
	Inode* inode = m_pfm->NameI(src, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "目标文件不存在！" << endl;
		m_itable->IPut(inode); return;
	}
	//条件2:必须是自己的文件
	if (inode->m_pinode->i_uid != inode->m_pinode->i_uid) {
		cout << "不是当前用户的文件，不允许修改权限！" << endl;
		m_itable->IPut(inode); return;
	}
	//条件3 文件未被打开
	if (inode->m_count != 0) {
		cout << "文件已被打开！请先关闭文件！" << endl;
		m_itable->IPut(inode); return;
	}
	//直接改
	if (is_add) {
		inode->m_pinode->i_mode |= mode;
	}
	else {
		inode->m_pinode->i_mode &= (~mode);
	}
	cout << "文件权限修改成功！" << endl;

	m_itable->IPut(inode);
}

void Ljyux::setmod(string src, int mode) {
	//条件1:路径存在
	int status;
	Inode* inode = m_pfm->NameI(src, status, DATA_FILE);
	if (status != DIR_EXIST) {
		cout << "目标文件不存在！" << endl;
		m_itable->IPut(inode); return;
	}
	//条件2:必须是自己的文件
	if (inode->m_pinode->i_uid != inode->m_pinode->i_uid) {
		cout << "不是当前用户的文件，不允许设置权限！" << endl;
		m_itable->IPut(inode); return;
	}
	//条件3 文件未被打开
	if (inode->m_count != 0) {
		cout << "文件已被打开！请先关闭文件！" << endl;
		m_itable->IPut(inode); return;
	}
	//直接改
	inode->m_pinode->i_mode = mode;
	cout << "文件权限修改成功！" << endl;

	m_itable->IPut(inode);
}

void Ljyux::whoami() {
	//输出用户信息即可
	if (m_user == nullptr)
		return;
	cout << "当前用户 : " << m_user->m_uinfo->m_name << endl;
	cout << "用户uid : " << m_user->m_uinfo->m_uid << endl;
	cout << "用户gid : " << m_user->m_uinfo->m_gid << endl;
}

void Ljyux::adduser(string uname, string pswd) {
	//先判断是否能加入
	//条件1:当前用户必须是root用户
	if (!is_root(m_user)) {
		cout << "只有root用户具有此权限！" << endl;
		return;
	}
	//条件2:用户不存在
	if (m_utable->UGet(uname) != nullptr) {
		cout << "用户已存在！" << endl;
		return;
	}
	//条件3:用户表未满
	User* user = m_utable->UAlloc();
	if (user == nullptr) {
		cout << "用户数目已达到系统上限！" << endl;
		return;
	}
	//满足后，可加入，设置一些参数
	strcpy(user->m_uinfo->m_name, uname.c_str());
	strcpy(user->m_uinfo->m_passwd, pswd.c_str());
	
	cout << "添加用户" << uname << "成功！";
	cout << "uid: " << user->m_uinfo->m_uid << " ";
	cout << "gid: " << user->m_uinfo->m_gid << endl;
}

void Ljyux::deluser(string uname) {
	//先判断是否能删除
	//条件1:当前用户必须是root用户
	if (!is_root(m_user)) {
		cout << "只有root用户具有此权限！" << endl;
		return;
	}
	//条件2:待删除用户存在且不能是root用户
	User* user = m_utable->UGet(uname);
	if (user == nullptr) {
		cout << "用户不存在！" << endl;
		return;
	}
	else if (is_root(user)) {
		cout << "不允许删除root用户！" << endl;
		return;
	}
	//满足后，可以删除
	m_utable->UDelete(user);

	cout << "删除用户" << uname << "成功！" << endl;
}

void Ljyux::su(string uname) {
	//先判断用户是否存在
	User* user = m_utable->UGet(uname);
	if (user == nullptr) {
		cout << "用户不存在！" << endl;
		return;
	}
	//若存在，输入密码
	cout << "请输入密码 : ";
	string pswd; cin >> pswd;
	if (pswd != user->m_uinfo->m_passwd) {
		cout << "密码错误！" << endl;
		return;
	}
	while (getchar() != '\n')continue;
	m_user = user;
	cout << "修改用户成功！当前用户为: " << uname << endl;
}

void Ljyux::setgroup(string uname, int gid) {
	//先判断是否能修改gid
	//条件1:当前用户必须是root用户
	if (!is_root(m_user)) {
		cout << "只有root用户具有此权限！" << endl;
		return;
	}
	//条件2:用户存在
	User* user = m_utable->UGet(uname);
	if (user == nullptr) {
		cout << "用户不存在" << endl;
		return;
	}
	user->m_uinfo->m_gid = gid;
	cout << "修改group id成功！" << endl;
}

void Ljyux::lsuser() {
	//从m_utable中取出所有用户
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