#include "FileManager.h"
FileManager::FileManager() {
	//ָ��ȫΪ����
	m_pfs = g_fs;
	m_ptable = g_ftable;
	m_itable = g_itable;
	//��Ŀ¼Inode��Ҫ�Լ�ȡ������Ŀ¼InodeΪ0
	//m_RootInode = m_itable->IGet(0);
	//m_RootInode->m_pinode->i_type = DIRECTORY_FILE;'
	m_RootInodeid = 0;
	//��ʼ��Ŀ¼��Root���������ú����ӿڸ�
	//m_curInode = m_itable->IGet(0);
	//m_RootInode->m_pinode->i_type = DIRECTORY_FILE;
	m_curInodeid = 0;
}

FileManager::~FileManager() {
	//��������ָ��Ϊ�����ͣ�����Ҫ�ͷ�
	//delete m_RootInode;
	//delete m_curInode;
}

/*Ŀ¼��غ����ĸ�������*/

//Ŀ¼�ļ������������Ҳ�����DirEntry��inode��Ϊ-1
DirEntry FileManager::search(Inode* inode, string name,int type) {
	DirEntry entry = { -1,{0} };
	DirBlock block;
	int rsize = 0;
	while (rsize < inode->m_pinode->i_size) {
		//��Ŀ¼�������
		int temp = inode->ReadI(reinterpret_cast<char*>(&block), 
							  rsize, BLOCK_SIZE);
		rsize += temp;
		for (int i = 0; i < DIR_NUM && i<temp/sizeof(DirEntry); i++) {
			if (name != block.m_entry[i].m_dname)
				continue;
			//���Ƕ�����Ŀ¼inode�����ж��Ƿ�Ϊ��Ŀ¼
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



//Ŀ¼�ļ���д�� . Ŀ¼��
void FileManager::mk_curdir(Inode* inode) {
	DirEntry entry;
	strcpy(entry.m_dname, ".");
	entry.m_inode = inode->m_pinode->i_inode_id;
	inode->WriteI(reinterpret_cast<char*>(&entry),
				  inode->m_pinode->i_size, sizeof(entry));
	inode->m_pinode->i_mode = 0x777;
}

//Ŀ¼�ļ���д�� .. Ŀ¼��
void FileManager::mk_lastdir(Inode* inode, int inode_id) {
	DirEntry entry;
	strcpy(entry.m_dname, "..");
	entry.m_inode = inode_id;
	inode->WriteI(reinterpret_cast<char*>(&entry),
				  inode->m_pinode->i_size, sizeof(entry));
	inode->m_pinode->i_mode = 0x777;
}

//��ȡȫ��Ŀ¼��
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

//�ж�һ���ļ��Ƿ�Ϊ���ļ�
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

/*��ʼ�����*/
//��ʽ��������������Ŀ¼�ļ�
void FileManager::fformat() {
	//�ļ�д��ĸ�ʽ��
	m_pfs->fformat();
	//������Ŀ¼�ļ�

	m_RootInodeid = m_curInodeid = m_pfs->Ialloc();
	Inode* RootInode = m_itable->IGet(m_RootInodeid);
	RootInode->m_pinode->i_type = DIRECTORY_FILE;
	mk_curdir(RootInode);
	m_itable->IPut(RootInode);

}

/*Ŀ¼���*/
//Ŀ¼����������·����Ӧ��Inode�����������������÷�ʽ����
Inode* FileManager::NameI(string fp,int& status,int type) {
	//ȡ����ʼĿ¼
	//Inode* curinode = fp[0] == '/' ? m_RootInode : m_curInode;
	//Inode* curinode = fp[0] == '/' ?
		//m_RootInode : m_curInode;
	Inode* curinode = fp[0] == '/' ? m_itable->IGet(m_RootInodeid) :m_itable->IGet(m_curInodeid);
	//�ָ���ַ���
	vector<string> tokens = split_fp(fp);
	//���
	bool flag = true;
	//��һ�ж�
	u_int i = 0;
	for (i = 0; i < tokens.size(); i++) {
		//�жϵ�ǰInode�ǲ���Ŀ¼�ļ�
		if (curinode->m_pinode->i_type != InodeType::DIRECTORY_FILE) {
			flag = false;
			break;
		}
		//������Ŀ¼�ļ���ȡ����Ѱ��
		DirEntry entry;
		if (type == DATA_FILE && i == tokens.size() - 1) {
			entry = search(curinode, tokens[i], type);
		}
		else {
			entry = search(curinode, tokens[i]);
		}
		//���û�ҵ���ʵ����entry��inode��Ϊ-1
		if (entry.m_inode == -1) {
			flag = false; break;
		}
		//�ҵ��Ļ�����һ��curinode
		m_itable -> IPut(curinode);
		curinode = m_itable->IGet(entry.m_inode);
	}
	//����ʧ��ʱ��flag==false
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

//��Ŀ¼�ļ������һ��Ŀ¼
void FileManager::AddDirEntry(Inode* inode, string dir) {
	//�涨��Ŀ¼�ļ���.Ϊ��ǰĿ¼��..Ϊ��һ��Ŀ¼
	//Ŀ¼���������Զ���Ϊ.��..��Ҳ��������������֣�
	//���mkdir��������dir=="."|".."
	//����һ���µ�inode
	int inode_id = m_pfs->Ialloc();
	if (inode_id < 0 || inode_id>= INODE_MAXNUM) {
		cout << "����Inode���㣬�޷�������" << endl;
		return;
	}
	DirEntry entry;
	strcpy(entry.m_dname, dir.c_str());
	entry.m_inode = inode_id;
	//����Inode�����޸���Ӧ����
	Inode* new_inode = m_itable->IGet(inode_id);
	new_inode->m_pinode->i_type = DIRECTORY_FILE;
	new_inode->m_pinode->i_inode_id = inode_id;
	//��Ŀ¼Ҫ���.��..��
	mk_curdir(new_inode);
	mk_lastdir(new_inode, inode->m_pinode->i_inode_id);
	new_inode->dump();
	//д�����
	inode->WriteI(reinterpret_cast<char*>(&entry), 
					  inode->m_pinode->i_size, sizeof(entry));
	inode->dump();
	m_itable->IPut(new_inode);
}

//ɾȥĿ¼�ļ��е�һ��Ŀ¼
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

//�г�Ŀ¼�����ļ���
void FileManager::ls_(Inode* inode) {
	vector<DirEntry> dirs = getDirs(inode);
	int cnt = 1;
	for (auto& dir : dirs) {
		cout << dir.m_dname << " ";
		if (cnt++ % 8 == 0)
			cout << endl;
	}
}

//�г�Ŀ¼�����ļ��ľ�����Ϣ
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

//�޸ĵ�ǰinode
void FileManager::chinode(Inode* inode) {
	m_curInodeid = inode->m_id;
}

/*�����ļ���д���*/
//�����ļ�
Inode* FileManager::fcreate(int imode) {
	//����һ��Inode
	int inode_id = m_pfs->Ialloc();
	Inode* inode = m_itable->IGet(inode_id);
	//�޸�Inode�������
	inode->m_pinode->i_mode = imode;
	inode->m_pinode->i_type = DATA_FILE;
	inode->m_pinode->i_size = 0;
	
	return inode;
}

//�����ļ������ɾ��
void FileManager::AddFile(Inode* father, Inode* child,string name) {
	DirEntry entry;
	entry.m_inode = child->m_id;
	strcpy(entry.m_dname, name.c_str());
	father->WriteI(reinterpret_cast<char*>(&entry),
				   father->m_pinode->i_size, sizeof(entry));
	father->dump();
}