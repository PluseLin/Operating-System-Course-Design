#include "Inode.h"
#include "DiskDevice.h"
#include "FileSystem.h"
Inode::Inode() {
	m_pinode = new DiskInode;
	m_Disk = g_Disk;
	m_pfs = g_fs;
	m_count = 0;
	m_id = -1;
}

Inode::Inode(int inode_id) {

	//取出DiskInode
	m_pinode = new DiskInode;
	//m_Disk = g_Disk;

	//m_pfs = g_fs;
	//
	m_count = 0;
	m_id = inode_id;
	load(inode_id);
}

Inode::Inode(const Inode& inode) {
	//m_Disk = g_Disk;
	//m_pfs = g_fs;

	m_count = 0;
	m_id = inode.m_id;

	m_pinode = new DiskInode;
	memcpy(m_pinode, inode.m_pinode, sizeof(DiskInode));
}

Inode::~Inode() {
	//dump();
	delete m_pinode;
}


//Inode读出与写入
void Inode::load(int inode_id) {
	if (inode_id < 0) inode_id = m_id;
	m_Disk->read(INODE_BLOCK_START, inode_id * sizeof(DiskInode), m_pinode);
	m_pinode->i_inode_id = inode_id;
	m_count = 0;
}

void Inode::dump(int inode_id) {
	if (inode_id < 0) inode_id = m_id;
	m_Disk->write(INODE_BLOCK_START, inode_id * sizeof(DiskInode), m_pinode);
}

//盘块映射
int Inode::Bmap(int lbn) {
	//Ljyux的二级索引结构与Unix V6++相同
	//0-5 0级索引	
	//lbn -> pbn 范围 [0,5]
	//6-7 1级索引 指向一个索引块（128条），每条指向一个盘块
	//lbn -> index -> pbn 范围 [6,6+128*2-1]
	//8-9 2级索引 指向一个索引块（128条），每条指向一个索引盘块
	//lbn ->index  -> index -> pbn 范围 [6+128*2,6+128*2+128*128*2-1]
	const int level0_index_max = 5;
	const int level1_index_max = 6 - 1 + 128 * 2;
	const int level2_index_max = 6 + 128 * 2 + 128 * 128 * 2 - 1;

	int res = 0;
	Block buf;

	if (lbn < 0) {
		//报错提示
		return -1;
	}
	else if (lbn >= 0 && lbn <= level0_index_max) {
		res = m_pinode->i_addr[lbn];
		//如果不幸发现遇到的是0，这时候需要扩展
		if (res == 0) {
			res = m_pinode->i_addr[lbn] = g_fs->Dalloc();
			m_pinode->i_addr[lbn];
			//更新DiskInode
			m_Disk->write(INODE_BLOCK_START, m_pinode->i_inode_id * sizeof(DiskInode), m_pinode);
		}
	}
	else if (lbn > level0_index_max && lbn <= level1_index_max) {
		int lv1_index = (lbn - level0_index_max - 1) / 128 + 6;
		int lv0_index = (lbn - level0_index_max - 1) % 128;
		int lv1_addr = 0;
		IndexBlock index;
		//先找出1级索引块
		//如果1级索引取出来是0，则需要申请盘块用于放索引。。。
		res = m_pinode->i_addr[lv1_index];
		if (res == 0) {
			res = m_pinode->i_addr[lv1_index] = m_pfs->Dalloc();
			m_Disk->write(INODE_BLOCK_START, m_pinode->i_inode_id * sizeof(DiskInode), m_pinode);
		}
		//再找0级索引块
		//先读出，在判断
		m_Disk->read(res, 0, &index);
		lv1_addr = res;
		res = index.m_index[lv0_index];
		if (res == 0) {
			res=index.m_index[lv0_index] = m_pfs->Dalloc();
			m_Disk->write(lv1_addr, 0, &index);
			
		}
	}
	else if (lbn > level1_index_max && lbn <= level2_index_max) {
		int lv2_index = (lbn - level1_index_max - 1) / (128 * 128) + 8;
		int lv1_index = (lbn - level1_index_max - 1) / 128;
		int lv0_index = (lbn - level1_index_max - 1) % 128;
		int lv2_addr = 0;
		int lv1_addr = 0;
		IndexBlock index1, index2;
		//先找出2级索引块，再判断
		res = m_pinode->i_addr[lv2_index];
		if (res == 0) {
			res = m_pinode->i_addr[lv2_index] = m_pfs->Dalloc();
			m_Disk->write(INODE_BLOCK_START, m_pinode->i_inode_id * sizeof(DiskInode), m_pinode);
		}
		//再找出1级索引块，再判断
		m_Disk->read(res, 0, &index2);
		lv2_addr = res;
		res = index2.m_index[lv1_index];
		if (res == 0) {
			res = index2.m_index[lv1_index] = m_pfs->Dalloc();
			m_Disk->write(lv2_addr, 0, &index2);
		}
		//再找出0级索引块，再判断
		m_Disk->read(res, 0, &index1);
		lv1_addr = res;
		res = index1.m_index[lv0_index];
		if (res == 0) {
			res = index1.m_index[lv0_index] = m_pfs->Dalloc();
			m_Disk->write(lv1_addr, 0, &index1);
		}
	}
	else {
		return -1;
	}
	return res;
}
//读写inode
int Inode::ReadI(char* dst,int offset,int size) {
	Block buf;
	char* tmp = dst;
	int rsize = 0;
	int left_size = min(size, m_pinode->i_size - offset);
	while (left_size > 0) {
		//先利用para计算起始逻辑盘块
		int lbn = offset / 512;
		int pbn = Bmap(lbn);
		//取出之
		m_Disk->read(pbn, 0, &buf);
		//计算需要部分的起始地址和长度
		int start = offset % 512;
		int length = min(BLOCK_SIZE - start, min(left_size, m_pinode->i_size - offset));
		//移动到目标区
		memcpy(tmp, buf.data + start, length);
		//修改参数
		//请保证dst空间足够大！！！
		tmp += length;
		left_size -= length;
		rsize += length;
		offset += length;
	}
	//返回
	return rsize;
}

int Inode::WriteI(char* src, int offset, int size) {
	Block buf;
	char* tmp = src;
	int wsize = 0;
	int left_size = size;
	//循环，每次读512字节
	while (left_size > 0) {
		//先计算逻辑盘块
		int lbn = offset / 512;
		int pbn = Bmap(lbn);
		//写Inode时，需要考虑若每次写入不是从盘块开始字节写满
		//则需要先取出来再覆盖再写回去
		int start = offset % 512;
		int length = min(BLOCK_SIZE - start, left_size);
		if (start != 0 || length != BLOCK_SIZE) {
			m_Disk->read(pbn, 0, &buf);
		}
		//将目标移动到缓存区
		memcpy(buf.data + start, tmp, length);
		m_Disk->write(pbn, 0, &buf);
		//修改para
		//请保证m_Base空间足够大！！！
		left_size -= length;
		wsize += length;
		offset += length;
		tmp += length;
	}
	if (m_pinode->i_size < offset) {
		m_pinode->i_size = offset;
	}
	dump();
	return wsize;
}

void Inode::FreeDisk() {
	int lbn = 0;
	int pbn = 0;
	while (lbn * BLOCK_SIZE < m_pinode->i_size) {
		pbn = Bmap(lbn);
		m_pfs->Dfree(pbn);
		lbn++;
	}
	m_pinode->i_size = 0;
	memset(m_pinode->i_addr, 0, sizeof(m_pinode->i_addr));
}

string Inode::authority() {
	string str;
	str += (m_pinode->i_type == DATA_FILE ? 'f' : 'd');

	str += (m_pinode->i_mode & USER_R ? 'r' : '-');
	str += (m_pinode->i_mode & USER_W ? 'w' : '-');
	str += (m_pinode->i_mode & USER_X ? 'x' : '-');
	str += (m_pinode->i_mode & GROUP_R ? 'r' : '-');
	str += (m_pinode->i_mode & GROUP_W ? 'w' : '-');
	str += (m_pinode->i_mode & GROUP_X ? 'x' : '-');
	str += (m_pinode->i_mode & OTHER_R ? 'r' : '-');
	str += (m_pinode->i_mode & OTHER_W ? 'w' : '-');
	str += (m_pinode->i_mode & OTHER_X ? 'x' : '-');

	return str;
}

InodeTable::InodeTable() {

}
InodeTable::~InodeTable() {
	for (int i = 0; i < NINODE; i++) {
		if (m_inodes[i].m_id != -1) {
			m_inodes[i].dump();
		}
	}
}

void InodeTable::clear() {
	for (int i = 0; i < NINODE; i++) {
		m_inodes[i].m_count = 0;
		m_inodes[i].m_id = -1;
		if (m_inodes[i].m_pinode != nullptr) 
			delete m_inodes[i].m_pinode;
		m_inodes[i].m_pinode = nullptr;
	}
}

Inode* InodeTable::IFind(int inode_id) {
	for (int i = 0; i < NINODE; i++) {
		if (m_inodes[i].m_id == inode_id) {
			return m_inodes + i;
		}
	}
	return nullptr;
}

Inode* InodeTable::IGet(int inode_id) {
	for (int i = 0; i < NINODE; i++) {
		if (m_inodes[i].m_id == -1) {
			m_inodes[i].m_id = inode_id;
			m_inodes[i].m_pinode = new DiskInode();
			m_inodes[i].load(inode_id);
			return m_inodes + i;
		}
	}
	return nullptr;
}

void InodeTable::IPut(Inode* pinode) {
	if (pinode == nullptr) return;
	pinode->dump(pinode->m_id);
	pinode->m_id = -1;
}
