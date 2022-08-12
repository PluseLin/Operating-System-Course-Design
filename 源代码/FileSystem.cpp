#include "FileSystem.h"
#include "DiskDevice.h"

void FileSystem::fformat_osblock() {
	//д��16���û���һ��Ϊroot������Ϊ��
	//����Root�û�
	UserInfo uinfo[USER_MAXNUM];
	strcpy(uinfo[0].m_name, "root");
	strcpy(uinfo[0].m_passwd, "root123");
	uinfo[0].m_uid = 0;
	uinfo[0].m_gid = 0;
	//д��
	m_Disk->write(OS_BLOCK_START, 0, uinfo, BLOCK_SIZE);
}

void FileSystem::fformat_spb() {
	//��ʽ��SuperBlock
	m_spb->s_isize = INODE_MAXNUM;
	m_spb->s_ninode = 100;

	m_spb->s_fsize = DISK_BLOCK_NUM - SPB_BLOCK_NUM - INODE_BLOCK_NUM;
	m_spb->s_nfree = BLOCK_GROUP_SIZE;
	//ֱ�ӹ���Ŀ����̿��spb+inode��ʼ
	int spb_start = OS_BLOCK_NUM + SPB_BLOCK_NUM + INODE_BLOCK_NUM;
	for (int i = 0; i < m_spb->s_nfree; i++) {
		m_spb->s_free[i] = i + spb_start;
	}
	//ֱ�ӹ���Ŀ���Inode��0��ʼ����ʼ��ջʽ�������99-0�ݼ�
	for (int i = 0; i < 100; i++) {
		m_spb->s_inode[i] = 99 - i;
	}
	memset(m_spb->padding, 0, sizeof(m_spb->padding));
	m_Disk->write(SPB_BLOCK_START, 0, m_spb);
}

void FileSystem::fformat_inode() {
	//��ʽ��Inode
	//ȫ��д��ռ���
	Block buf;
	for (int i = INODE_BLOCK_START; i < INODE_BLOCK_NUM + INODE_BLOCK_START; i++) {
		m_Disk->write(i, 0, &buf);
	}
}

void FileSystem::fformat_data() {
	//��ʽ�������̿�
	//С�鳤д���̿鼴��
	const int data_start = OS_BLOCK_NUM+SPB_BLOCK_NUM + INODE_BLOCK_NUM;
	for (int blk_id = data_start; blk_id < DISK_BLOCK_NUM; blk_id += BLOCK_GROUP_SIZE) {
		//��GROUP_SIZEдһ��
		BlockGroup group;
		group.g_nblock = min(BLOCK_GROUP_SIZE, DISK_BLOCK_NUM - blk_id);
		//���磺�鳤130�̿��У���һ����ϢΪ230-329
		//���һ������д����100�����򳬳�����д0
		for (int i = 0; i < BLOCK_GROUP_SIZE; i++) {
			group.g_block[i] = (blk_id + BLOCK_GROUP_SIZE + i < DISK_BLOCK_NUM) ?
				blk_id + BLOCK_GROUP_SIZE + i : 0;
		}
		//д�뵽������
		m_Disk->write(blk_id, 0, &group);
	}
}

void FileSystem::Search_FreeInode() {
	int blk_id = INODE_BLOCK_START;
	//ÿ���̿�8��dinode
	DiskInode dinode[8];
	Block buf;
	
	while (m_spb->s_ninode < 100 && blk_id < INODE_BLOCK_START + INODE_BLOCK_NUM) {
		m_Disk->read(blk_id, 0, &buf);
		memcpy(dinode, &buf, sizeof(dinode));
		for (int i = 0; i < 8 && m_spb->s_ninode < 100 ; i++) {
			if (dinode[i].i_inode_id == 0 && dinode[i].i_type == 0) {
				m_spb->s_inode[m_spb->s_ninode++] = (blk_id - INODE_BLOCK_START) * 8 + i;
			}
		}
		blk_id++;
	}
}

FileSystem::FileSystem() {
	m_spb = new SuperBlock;
	m_Disk = g_Disk;
	m_Disk->read(SPB_BLOCK_START, 0, m_spb);
}

FileSystem::~FileSystem() {
	m_Disk->write(SPB_BLOCK_START, 0, m_spb);
	delete m_spb;
}

//�����̿���������ͷ�
int FileSystem::Dalloc() {
	//�������ӷ�����
	int blk_id = m_spb->s_free[--m_spb->s_nfree];
	//��������������0�ţ���ʵ���Ѿ��������ˣ�Ҫ���ռ䲻��Ĵ�
	if (blk_id == 0) {
		cout << "�̿���ȫ��������!" << endl;
		throw   1;
		return -1;
	}
	//�����ǰ����Ŀ����̿鶼�������ˣ���Ҫ������̿��С����Ϣ���뵽spb
	if (m_spb->s_nfree == 0) {
		//cout << blk_id << endl;
		BlockGroup g;
		m_Disk->read(blk_id, 0, &g);
		m_spb->s_nfree = g.g_nblock;
		for (int i = 0; i < BLOCK_GROUP_SIZE; i++) {
			m_spb->s_free[i] = g.g_block[i];
		}
	}
	return blk_id;
}

void FileSystem::Dfree(int blk_id) {
	//�����ǰ����Ŀ����̿��Ѿ����ˣ����С����Ϣд�뵽���յ��̿�
	if (m_spb->s_nfree == BLOCK_GROUP_SIZE) {
		BlockGroup g;
		g.g_nblock = m_spb->s_nfree;
		for (int i = 0; i < BLOCK_GROUP_SIZE; i++) {
			g.g_block[i] = m_spb->s_free[i];
		}
		m_Disk->write(blk_id, 0, &g);
		m_spb->s_nfree = 0;
	}
	//�������ӷ��ͷ�
	m_spb->s_free[m_spb->s_nfree++] = blk_id;
}

//����Inode���������ͷ�
int FileSystem::Ialloc() {
	//���̿����������100�������̿�
	if (m_spb->s_ninode == 0) {
		Search_FreeInode();
		if (m_spb->s_ninode == 0) {
			cout << "����Inode���㣬�޷������ļ���" << endl;
			throw 2;
			return -1;
		}
	}
	return m_spb->s_inode[--m_spb->s_ninode];
}

void FileSystem::IFree(int inode_id) {
	if (m_spb->s_ninode < 100) {
		m_spb->s_inode[m_spb->s_ninode++] = inode_id;
	}
	DiskInode inode;
	m_Disk->write(INODE_BLOCK_START, inode_id * sizeof(DiskInode), &inode);
}

//��ʽ�����
void FileSystem::fformat() {
	m_Disk->create_null_file(disk_fp, DISK_BLOCK_NUM);
	fformat_osblock();
	fformat_spb();
	fformat_inode();
	fformat_data();

}