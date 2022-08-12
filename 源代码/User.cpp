#include "User.h"
User::User() {
	m_uinfo = new UserInfo;
	m_files = new OpenFiles;
}

User::~User() {
	delete m_files;
	delete m_uinfo;
}

void User::clear() {
	m_inodemap.clear();
	m_uinfo->m_uid = -1;
	m_uinfo->m_gid = -1;
	memset(m_uinfo->m_name, 0, sizeof(m_uinfo->m_name));
	memset(m_uinfo->m_passwd, 0, sizeof(m_uinfo->m_passwd));
}

void User::setuid(short uid) {
	m_uinfo->m_uid = uid;
}

void User::setgid(short gid) {
	m_uinfo->m_gid = gid;
}

int User::getfd(int inode_id) {
	auto res = m_inodemap.find(inode_id);
	return res == m_inodemap.end() ? -1 : res->second;
}

void User::setfd(int fd, int inode_id) {
	m_inodemap[inode_id] = fd;
}

void User::freefd(int fd) {
	m_inodemap.erase(fd);
	//m_files->FreeF(fd);
}

UserTable::UserTable() {
	reload();
}

void UserTable::reload() {
	UserBlock ublock;
	//从硬盘中读取
	g_Disk->read(OS_BLOCK_START, 0, &ublock);
	//分配
	for (int i = 0; i < USER_MAXNUM; i++) {
		memcpy(m_Users[i].m_uinfo, ublock.m_user + i, sizeof(UserInfo));
	}
}

UserTable::~UserTable() {
	//写回硬盘
	UserBlock ublock;
	for (int i = 0; i < USER_MAXNUM; i++) {
		memcpy(ublock.m_user + i, m_Users[i].m_uinfo, sizeof(UserInfo));
	}
	g_Disk->write(OS_BLOCK_START, 0, &ublock);
}

void UserTable::clear() {
	for (int i = 0; i < USER_MAXNUM; i++) {
		memset(m_Users[i].m_uinfo, 0, sizeof(UserInfo));
	}
}

User* UserTable::UGet(string uname) {
	for (int i = 0; i < USER_MAXNUM; i++) {
		if (m_Users[i].m_uinfo->m_name == uname) {
			return m_Users + i;
		}
	}
	return nullptr;
}

User* UserTable::UGet(int uid) {
	for (int i = 0; i < USER_MAXNUM; i++) {
		if (m_Users[i].m_uinfo->m_uid == uid) {
			return m_Users + i;
		}
	}
	return nullptr;
}

User* UserTable::UAlloc() {
	for (int i = 0; i < USER_MAXNUM; i++) {
		if (m_Users[i].m_uinfo->m_uid == -1) {
			m_Users[i].m_uinfo->m_uid = i;
			m_Users[i].m_uinfo->m_gid = 0;
			return m_Users + i;
		}
	}
	return nullptr;
}

void UserTable::UDelete(User* user) {
	if (user==nullptr ||user->m_uinfo->m_uid == 0) return;
	user->clear();
}

vector<User*> UserTable::GetAllUser() {
	vector<User*> users;

	for (int i = 0; i < USER_MAXNUM; i++) {
		if (m_Users[i].m_uinfo->m_uid != -1) {
			users.push_back(m_Users + i);
		}
	}

	return users;
}