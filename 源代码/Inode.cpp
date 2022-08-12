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

	//ȡ��DiskInode
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


//Inode������д��
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

//�̿�ӳ��
int Inode::Bmap(int lbn) {
	//Ljyux�Ķ��������ṹ��Unix V6++��ͬ
	//0-5 0������	
	//lbn -> pbn ��Χ [0,5]
	//6-7 1������ ָ��һ�������飨128������ÿ��ָ��һ���̿�
	//lbn -> index -> pbn ��Χ [6,6+128*2-1]
	//8-9 2������ ָ��һ�������飨128������ÿ��ָ��һ�������̿�
	//lbn ->index  -> index -> pbn ��Χ [6+128*2,6+128*2+128*128*2-1]
	const int level0_index_max = 5;
	const int level1_index_max = 6 - 1 + 128 * 2;
	const int level2_index_max = 6 + 128 * 2 + 128 * 128 * 2 - 1;

	int res = 0;
	Block buf;

	if (lbn < 0) {
		//������ʾ
		return -1;
	}
	else if (lbn >= 0 && lbn <= level0_index_max) {
		res = m_pinode->i_addr[lbn];
		//������ҷ�����������0����ʱ����Ҫ��չ
		if (res == 0) {
			res = m_pinode->i_addr[lbn] = g_fs->Dalloc();
			m_pinode->i_addr[lbn];
			//����DiskInode
			m_Disk->write(INODE_BLOCK_START, m_pinode->i_inode_id * sizeof(DiskInode), m_pinode);
		}
	}
	else if (lbn > level0_index_max && lbn <= level1_index_max) {
		int lv1_index = (lbn - level0_index_max - 1) / 128 + 6;
		int lv0_index = (lbn - level0_index_max - 1) % 128;
		int lv1_addr = 0;
		IndexBlock index;
		//���ҳ�1��������
		//���1������ȡ������0������Ҫ�����̿����ڷ�����������
		res = m_pinode->i_addr[lv1_index];
		if (res == 0) {
			res = m_pinode->i_addr[lv1_index] = m_pfs->Dalloc();
			m_Disk->write(INODE_BLOCK_START, m_pinode->i_inode_id * sizeof(DiskInode), m_pinode);
		}
		//����0��������
		//�ȶ��������ж�
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
		//���ҳ�2�������飬���ж�
		res = m_pinode->i_addr[lv2_index];
		if (res == 0) {
			res = m_pinode->i_addr[lv2_index] = m_pfs->Dalloc();
			m_Disk->write(INODE_BLOCK_START, m_pinode->i_inode_id * sizeof(DiskInode), m_pinode);
		}
		//���ҳ�1�������飬���ж�
		m_Disk->read(res, 0, &index2);
		lv2_addr = res;
		res = index2.m_index[lv1_index];
		if (res == 0) {
			res = index2.m_index[lv1_index] = m_pfs->Dalloc();
			m_Disk->write(lv2_addr, 0, &index2);
		}
		//���ҳ�0�������飬���ж�
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
//��дinode
int Inode::ReadI(char* dst,int offset,int size) {
	Block buf;
	char* tmp = dst;
	int rsize = 0;
	int left_size = min(size, m_pinode->i_size - offset);
	while (left_size > 0) {
		//������para������ʼ�߼��̿�
		int lbn = offset / 512;
		int pbn = Bmap(lbn);
		//ȡ��֮
		m_Disk->read(pbn, 0, &buf);
		//������Ҫ���ֵ���ʼ��ַ�ͳ���
		int start = offset % 512;
		int length = min(BLOCK_SIZE - start, min(left_size, m_pinode->i_size - offset));
		//�ƶ���Ŀ����
		memcpy(tmp, buf.data + start, length);
		//�޸Ĳ���
		//�뱣֤dst�ռ��㹻�󣡣���
		tmp += length;
		left_size -= length;
		rsize += length;
		offset += length;
	}
	//����
	return rsize;
}

int Inode::WriteI(char* src, int offset, int size) {
	Block buf;
	char* tmp = src;
	int wsize = 0;
	int left_size = size;
	//ѭ����ÿ�ζ�512�ֽ�
	while (left_size > 0) {
		//�ȼ����߼��̿�
		int lbn = offset / 512;
		int pbn = Bmap(lbn);
		//дInodeʱ����Ҫ������ÿ��д�벻�Ǵ��̿鿪ʼ�ֽ�д��
		//����Ҫ��ȡ�����ٸ�����д��ȥ
		int start = offset % 512;
		int length = min(BLOCK_SIZE - start, left_size);
		if (start != 0 || length != BLOCK_SIZE) {
			m_Disk->read(pbn, 0, &buf);
		}
		//��Ŀ���ƶ���������
		memcpy(buf.data + start, tmp, length);
		m_Disk->write(pbn, 0, &buf);
		//�޸�para
		//�뱣֤m_Base�ռ��㹻�󣡣���
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
