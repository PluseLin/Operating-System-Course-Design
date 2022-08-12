#pragma once
#include"header.h"
#include"DiskDevice.h"


/*
UserInfo结构体，存入内存中，共32B
*/
struct UserInfo {
	short		m_uid = -1;
	short		m_gid = -1;
	char		m_name[USERNAME_MAXLEN + 1] = { 0 };
	char		m_passwd[PASSWD_MAXLEN + 1] = { 0 };
};

/*
InodeMode，枚举类型，用于一个inode的读写执行权限表示
*/
enum InodeMode {
	USER_R = 0x400,
	USER_W = 0x200,
	USER_X = 0x100,
	GROUP_R = 0x040,
	GROUP_W = 0x020,
	GROUP_X = 0x010,
	OTHER_R = 0x004,
	OTHER_W = 0x002,
	OTHER_X = 0x001
};

enum UseMode {
	UREAD = 0x4,
	UWRITE = 0x2,
	UEXE = 0x1
};

/*
InodeType，枚举类型，用于表示一个inode是普通文件还是目录文件
*/
enum InodeType {
	DATA_FILE = 0x01,
	DIRECTORY_FILE = 0x02,
};

/*
DiskInode，结构体，磁盘inode，32B
*/
struct DiskInode {
	int		i_addr[10] = { 0 };		//逻辑盘块到物理盘块映射
	int		i_mode = 0;				//Inode的读写权限
	int		i_size = 0;				//Inode对应的文件大小
	short	i_uid = -1;				//Inode持有者的用户的id
	short	i_gid = -1;				//Inode持有者的用户所在组的id
	int		i_type = 0;				//Inode对应的文件类型
	int		i_inode_id = 0;			//Inode编号
	int		d_mtime = 0;			//最后访问时间
};

/*
SuperBlock，结构体，对应磁盘SuperBlock区，1024B
*/
struct SuperBlock {
	//管理Inode
	int s_isize;				//inode区总数
	int s_ninode;				//空闲inode数量
	int s_inode[100];			//直接管理的空闲DiskInode

	//管理数据盘块
	int s_fsize;				//数据文件区盘块总数
	int s_nfree;				//直接管理的空闲盘块数量
	int s_free[100];			//直接管理的空闲盘块索引

	int padding[52];			//填充
};

/*BlockGroup，小组长，用于记录空闲盘块组信息*/
struct BlockGroup {
	int	g_nblock;				//磁盘块组数目
	int	g_block[100];			//磁盘块对应物理块编号
};

struct IndexBlock {
	int m_index[BLOCK_SIZE / 4] = { 0 };
};

/*
FileSystem，类，管理SuperBlock的操作，并负责格式化
*/
class FileSystem{
private:
	SuperBlock* m_spb;
	DiskDevice* m_Disk;

private:
	void fformat_spb();
	void fformat_inode();
	void fformat_data();
	void fformat_osblock();
	//在磁盘中搜索空闲DiskInode结构
	void Search_FreeInode();
public:
	FileSystem();
	~FileSystem();
	//空闲盘块的申请与释放
	int Dalloc();
	void Dfree(int blk_id);
	//空闲Inode的申请与释放
	int Ialloc();
	void IFree(int inode_id);

	//格式化相关
	void fformat();

	
};

extern FileSystem* g_fs;