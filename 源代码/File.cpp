#include "File.h"



File::File() {
	m_pinode = nullptr;
	m_inode_id = -1;
	m_fmode = 0;
	m_foffset = 0;
}

File::~File() {

}

void File::setF(Inode* pinode, int fmode) {
	m_pinode = pinode;
	m_inode_id = pinode->m_id;
	m_fmode = fmode;
	m_foffset = 0;
}

OpenFiles::OpenFiles() {
	
}

OpenFiles::~OpenFiles() {

}

//�����ļ����fd��ȡ�ļ�ָ��
File* OpenFiles::GetF(int fd) {
	if (fd < 0 || fd >= USER_OPENFILE_NUM) {
		return nullptr;
	}
	return m_FileTable[fd];
}

//���ļ����fd���ļ�ָ���
void OpenFiles::SetF(int fd, File* pfile) {
	if (fd >= 0 && fd < USER_OPENFILE_NUM) {
		m_FileTable[fd] = pfile;
	}
}

//����fd
int OpenFiles::AllocFd() {
	for (int i = 0; i < USER_OPENFILE_NUM; i++) {
		if (m_FileTable[i] == nullptr) {
			return i;
		}
	}
	return -1;
}

//�ͷ�fd
void OpenFiles::FreeFd(int fd) {
	if (fd >= 0 && fd < USER_OPENFILE_NUM) {
		m_FileTable[fd] = nullptr;
	}
}

//����Ƿ����ļ���
bool OpenFiles::has_openfile() {
	for (int i = 0; i < USER_OPENFILE_NUM; i++) {
		if (m_FileTable[i] != nullptr) {
			return true;
		}
	}
	return false;
}

OpenFileTable::OpenFileTable() {

}

OpenFileTable::~OpenFileTable() {

}

//����һ������File��
File* OpenFileTable::Falloc() {
	for (int i = 0; i < ALL_OPENFILE_NUM; i++) {
		if ((m_File + i)->m_pinode == nullptr) {
			return m_File + i;
		}
	}
	return nullptr;
}
//����һ������File��
void OpenFileTable::Ffree(File* pfile) {
	pfile->m_fmode = 0;
	pfile->m_foffset = 0;
	pfile->m_inode_id = 0;
	pfile->m_pinode = nullptr;
}

Inode* OpenFileTable::Ifind(int inode_id) {
	for (int i = 0; i < ALL_OPENFILE_NUM; i++) {
		if (m_File[i].m_inode_id == inode_id) {
			return m_File[i].m_pinode;
		}
	}
	return nullptr;
}