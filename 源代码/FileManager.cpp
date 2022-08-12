#include "FileManager.h"
FileManager::FileManager() {
	//指针全为引用
	m_pfs = g_fs;
	m_ptable = g_ftable;
	m_itable = g_itable;
	//根目录Inode需要自己取出，根目录Inode为0
	//m_RootInode = m_itable->IGet(0);
	//m_RootInode->m_pinode->i_type = DIRECTORY_FILE;'
	m_RootInodeid = 0;
	//初始根目录是Root，后续可用函数接口改
	//m_curInode = m_itable->IGet(0);
	//m_RootInode->m_pinode->i_type = DIRECTORY_FILE;
	m_curInodeid = 0;
}

FileManager::~FileManager() {
	//另外两个指针为引用型，不需要释放
	//delete m_RootInode;
	//delete m_curInode;
}

/*目录相关函数的辅助函数*/

//目录文件的搜索，若找不到，DirEntry中inode号为-1
DirEntry FileManager::search(Inode* inode, string name,int type) {
	DirEntry entry = { -1,{0} };
	DirBlock block;
	int rsize = 0;
	while (rsize < inode->m_pinode->i_size) {
		//将目录块读出来
		int temp = inode->ReadI(reinterpret_cast<char*>(&block), 
							  rsize, BLOCK_SIZE);
		rsize += temp;
		for (int i = 0; i < DIR_NUM && i<temp/sizeof(DirEntry); i++) {
			if (name != block.m_entry[i].m_dname)
				continue;
			//还是读出来目录inode，以判断是否为根目录
			Inode* dinode = m_itable->IGet(block.m_entry[i].m_inode);
			if (dinode->m_pinode->i_type==type) {
				//delete dinode;
				m_itable->IPut(dinode);
				return block.m_entry[i];
			}
			//delete dinode;
			m_itable->IPut(dinode);
		}
	}
	return entry;
}



//目录文件中写入 . 目录项
void FileManager::mk_curdir(Inode* inode) {
	DirEntry entry;
	strcpy(entry.m_dname, ".");
	entry.m_inode = inode->m_pinode->i_inode_id;
	inode->WriteI(reinterpret_cast<char*>(&entry),
				  inode->m_pinode->i_size, sizeof(entry));
	inode->m_pinode->i_mode = 0x777;
}

//目录文件中写入 .. 目录项
void FileManager::mk_lastdir(Inode* inode, int inode_id) {
	DirEntry entry;
	strcpy(entry.m_dname, "..");
	entry.m_inode = inode_id;
	inode->WriteI(reinterpret_cast<char*>(&entry),
				  inode->m_pinode->i_size, sizeof(entry));
	inode->m_pinode->i_mode = 0x777;
}

//获取全部目录项
vector<DirEntry> FileManager::getDirs(Inode* inode) {
	vector<DirEntry> dirs;
	DirBlock block;
	int rsize = 0;
	while (rsize < inode->m_pinode->i_size) {
		int temp = inode->ReadI(reinterpret_cast<char*>(&block),
								rsize, sizeof(block));
		rsize += temp;
		for (int i = 0; i < DIR_NUM && i < temp / sizeof(DirEntry); i++) {
			dirs.push_back(block.m_entry[i]);
		}
	}
	return dirs;
}

//判断一个文件是否为空文件
bool FileManager::inode_is_empty(Inode* inode) {
	if (inode->m_pinode->i_type == DATA_FILE) {
		return inode->m_pinode->i_size == 0;
	}
	vector<DirEntry> dirs = getDirs(inode);
	for (auto& dir : dirs){
		if (dir.m_dname != "." && dir.m_dname != "..") {
			return false;
		}
	}
	return true;
}

/*初始化相关*/
//格式化，包括建立根目录文件
void FileManager::fformat() {
	//文件写入的格式化
	m_pfs->fformat();
	//建立根目录文件

	m_RootInodeid = m_curInodeid = m_pfs->Ialloc();
	Inode* RootInode = m_itable->IGet(m_RootInodeid);
	RootInode->m_pinode->i_type = DIRECTORY_FILE;
	mk_curdir(RootInode);
	m_itable->IPut(RootInode);

}

/*目录相关*/
//目录搜索，返回路径对应的Inode，搜索错误性以引用方式返回
Inode* FileManager::NameI(string fp,int& status,int type) {
	//取出初始目录
	//Inode* curinode = fp[0] == '/' ? m_RootInode : m_curInode;
	//Inode* curinode = fp[0] == '/' ?
		//m_RootInode : m_curInode;
	Inode* curinode = fp[0] == '/' ? m_itable->IGet(m_RootInodeid) :m_itable->IGet(m_curInodeid);
	//分割出字符流
	vector<string> tokens = split_fp(fp);
	//标记
	bool flag = true;
	//逐一判断
	u_int i = 0;
	for (i = 0; i < tokens.size(); i++) {
		//判断当前Inode是不是目录文件
		if (curinode->m_pinode->i_type != InodeType::DIRECTORY_FILE) {
			flag = false;
			break;
		}
		//否则，是目录文件，取出并寻找
		DirEntry entry;
		if (type == DATA_FILE && i == tokens.size() - 1) {
			entry = search(curinode, tokens[i], type);
		}
		else {
			entry = search(curinode, tokens[i]);
		}
		//如果没找到，实际上entry的inode号为-1
		if (entry.m_inode == -1) {
			flag = false; break;
		}
		//找到的话，改一下curinode
		m_itable -> IPut(curinode);
		curinode = m_itable->IGet(entry.m_inode);
	}
	//搜索失败时，flag==false
	if (!flag) {
		if (i != tokens.size() - 1)
			status = FATHER_NOT_EXIST;
		else
			status = CURDIR_NOT_EXIST;
	}
	else {
		status = DIR_EXIST;
	}
	return curinode;
}

//在目录文件中添加一个目录
void FileManager::AddDirEntry(Inode* inode, string dir) {
	//规定：目录文件中.为当前目录，..为上一级目录
	//目录名不允许自定义为.和..，也不许改这两个名字！
	//因此mkdir函数不许dir=="."|".."
	//分配一个新的inode
	int inode_id = m_pfs->Ialloc();
	if (inode_id < 0 || inode_id>= INODE_MAXNUM) {
		cout << "空闲Inode不足，无法创建！" << endl;
		return;
	}
	DirEntry entry;
	strcpy(entry.m_dname, dir.c_str());
	entry.m_inode = inode_id;
	//创建Inode，并修改相应数据
	Inode* new_inode = m_itable->IGet(inode_id);
	new_inode->m_pinode->i_type = DIRECTORY_FILE;
	new_inode->m_pinode->i_inode_id = inode_id;
	//新目录要添加.和..项
	mk_curdir(new_inode);
	mk_lastdir(new_inode, inode->m_pinode->i_inode_id);
	new_inode->dump();
	//写入磁盘
	inode->WriteI(reinterpret_cast<char*>(&entry), 
					  inode->m_pinode->i_size, sizeof(entry));
	inode->dump();
	m_itable->IPut(new_inode);
}

//删去目录文件中的一个目录
void FileManager::RemoveDirEntry(Inode* inode, int del_id) {
	vector<DirEntry> dirs = getDirs(inode);
	u_int i = 0;
	DirEntry* new_dirs = new DirEntry[dirs.size()];
	int cnt = 0;
	for (i = 0; i < dirs.size();i++) {
		if (dirs[i].m_inode != del_id) {
			memcpy(new_dirs + cnt, &(dirs[i]), sizeof(DirEntry));
			cnt++;
		}
	}
	if (cnt == dirs.size()) return;
	inode->WriteI(reinterpret_cast<char*>(new_dirs),
				  0, cnt * sizeof(DirEntry));
	inode->m_pinode->i_size -= sizeof(DirEntry);
}

//列出目录所有文件名
void FileManager::ls_(Inode* inode) {
	vector<DirEntry> dirs = getDirs(inode);
	int cnt = 1;
	for (auto& dir : dirs) {
		cout << dir.m_dname << " ";
		if (cnt++ % 8 == 0)
			cout << endl;
	}
}

//列出目录所有文件的具体信息
void FileManager::ls_l(Inode* inode) {
	vector<DirEntry> dirs = getDirs(inode);
	int cnt = 1;
	//	name		size		type		owner_id	
	//	.			64B			DIR			0
	vector<string> title{
		"filemode","name","size","owner_uid"
	};
	for (auto& t : title) {
		cout << t << "\t\t";
	}
	cout << endl;
	for (auto& dir : dirs) {
		Inode* inode = m_itable->IGet(dir.m_inode);
		cout << inode->authority() << "\t\t";
		cout << dir.m_dname << "\t\t";
		if (inode->m_pinode->i_type == DATA_FILE) {
			cout << inode->m_pinode->i_size << "B" << "\t\t";
		}
		else {
			cout << inode->m_pinode->i_size / 64 << "\t\t";
		}
		cout << inode->m_pinode->i_uid << "\t\t";
		cout << endl;
		m_itable->IPut(inode);
	}
}

//修改当前inode
void FileManager::chinode(Inode* inode) {
	m_curInodeid = inode->m_id;
}

/*数据文件读写相关*/
//创建文件
Inode* FileManager::fcreate(int imode) {
	//申请一个Inode
	int inode_id = m_pfs->Ialloc();
	Inode* inode = m_itable->IGet(inode_id);
	//修改Inode相关内容
	inode->m_pinode->i_mode = imode;
	inode->m_pinode->i_type = DATA_FILE;
	inode->m_pinode->i_size = 0;
	
	return inode;
}

//数据文件添加与删除
void FileManager::AddFile(Inode* father, Inode* child,string name) {
	DirEntry entry;
	entry.m_inode = child->m_id;
	strcpy(entry.m_dname, name.c_str());
	father->WriteI(reinterpret_cast<char*>(&entry),
				   father->m_pinode->i_size, sizeof(entry));
	father->dump();
}