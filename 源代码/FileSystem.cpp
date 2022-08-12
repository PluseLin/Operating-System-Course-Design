#include "FileSystem.h"
#include "DiskDevice.h"

void FileSystem::fformat_osblock() {
	//写入16个用户，一个为root，其余为空
	//构建Root用户
	UserInfo uinfo[USER_MAXNUM];
	strcpy(uinfo[0].m_name, "root");
	strcpy(uinfo[0].m_passwd, "root123");
	uinfo[0].m_uid = 0;
	uinfo[0].m_gid = 0;
	//写入
	m_Disk->write(OS_BLOCK_START, 0, uinfo, BLOCK_SIZE);
}

void FileSystem::fformat_spb() {
	//格式化SuperBlock
	m_spb->s_isize = INODE_MAXNUM;
	m_spb->s_ninode = 100;

	m_spb->s_fsize = DISK_BLOCK_NUM - SPB_BLOCK_NUM - INODE_BLOCK_NUM;
	m_spb->s_nfree = BLOCK_GROUP_SIZE;
	//直接管理的空闲盘块从spb+inode开始
	int spb_start = OS_BLOCK_NUM + SPB_BLOCK_NUM + INODE_BLOCK_NUM;
	for (int i = 0; i < m_spb->s_nfree; i++) {
		m_spb->s_free[i] = i + spb_start;
	}
	//直接管理的空闲Inode从0开始，初始按栈式管理，因此99-0递减
	for (int i = 0; i < 100; i++) {
		m_spb->s_inode[i] = 99 - i;
	}
	memset(m_spb->padding, 0, sizeof(m_spb->padding));
	m_Disk->write(SPB_BLOCK_START, 0, m_spb);
}

void FileSystem::fformat_inode() {
	//格式化Inode
	//全部写入空即可
	Block buf;
	for (int i = INODE_BLOCK_START; i < INODE_BLOCK_NUM + INODE_BLOCK_START; i++) {
		m_Disk->write(i, 0, &buf);
	}
}

void FileSystem::fformat_data() {
	//格式化数据盘块
	//小组长写入盘块即可
	const int data_start = OS_BLOCK_NUM+SPB_BLOCK_NUM + INODE_BLOCK_NUM;
	for (int blk_id = data_start; blk_id < DISK_BLOCK_NUM; blk_id += BLOCK_GROUP_SIZE) {
		//隔GROUP_SIZE写一组
		BlockGroup group;
		group.g_nblock = min(BLOCK_GROUP_SIZE, DISK_BLOCK_NUM - blk_id);
		//例如：组长130盘块中，下一组信息为230-329
		//最后一组往往写不到100个，则超出部分写0
		for (int i = 0; i < BLOCK_GROUP_SIZE; i++) {
			group.g_block[i] = (blk_id + BLOCK_GROUP_SIZE + i < DISK_BLOCK_NUM) ?
				blk_id + BLOCK_GROUP_SIZE + i : 0;
		}
		//写入到磁盘中
		m_Disk->write(blk_id, 0, &group);
	}
}

void FileSystem::Search_FreeInode() {
	int blk_id = INODE_BLOCK_START;
	//每个盘块8个dinode
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

//空闲盘块的申请与释放
int FileSystem::Dalloc() {
	//成组连接法申请
	int blk_id = m_spb->s_free[--m_spb->s_nfree];
	//如果申请出来的是0号，其实是已经申请完了，要报空间不足的错
	if (blk_id == 0) {
		cout << "盘块已全部申请完!" << endl;
		throw   1;
		return -1;
	}
	//如果当前管理的空闲盘块都申请完了，需要将这个盘块的小组信息读入到spb
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
	//如果当前管理的空闲盘块已经满了，则把小组信息写入到回收的盘块
	if (m_spb->s_nfree == BLOCK_GROUP_SIZE) {
		BlockGroup g;
		g.g_nblock = m_spb->s_nfree;
		for (int i = 0; i < BLOCK_GROUP_SIZE; i++) {
			g.g_block[i] = m_spb->s_free[i];
		}
		m_Disk->write(blk_id, 0, &g);
		m_spb->s_nfree = 0;
	}
	//成组连接法释放
	m_spb->s_free[m_spb->s_nfree++] = blk_id;
}

//空闲Inode的申请与释放
int FileSystem::Ialloc() {
	//在盘块中搜索最多100个空闲盘块
	if (m_spb->s_ninode == 0) {
		Search_FreeInode();
		if (m_spb->s_ninode == 0) {
			cout << "空闲Inode不足，无法创建文件！" << endl;
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

//格式化相关
void FileSystem::fformat() {
	m_Disk->create_null_file(disk_fp, DISK_BLOCK_NUM);
	fformat_osblock();
	fformat_spb();
	fformat_inode();
	fformat_data();

}