#pragma once
#include"header.h"
#include"DiskDevice.h"


/*
UserInfo�ṹ�壬�����ڴ��У���32B
*/
struct UserInfo {
	short		m_uid = -1;
	short		m_gid = -1;
	char		m_name[USERNAME_MAXLEN + 1] = { 0 };
	char		m_passwd[PASSWD_MAXLEN + 1] = { 0 };
};

/*
InodeMode��ö�����ͣ�����һ��inode�Ķ�дִ��Ȩ�ޱ�ʾ
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
InodeType��ö�����ͣ����ڱ�ʾһ��inode����ͨ�ļ�����Ŀ¼�ļ�
*/
enum InodeType {
	DATA_FILE = 0x01,
	DIRECTORY_FILE = 0x02,
};

/*
DiskInode���ṹ�壬����inode��32B
*/
struct DiskInode {
	int		i_addr[10] = { 0 };		//�߼��̿鵽�����̿�ӳ��
	int		i_mode = 0;				//Inode�Ķ�дȨ��
	int		i_size = 0;				//Inode��Ӧ���ļ���С
	short	i_uid = -1;				//Inode�����ߵ��û���id
	short	i_gid = -1;				//Inode�����ߵ��û��������id
	int		i_type = 0;				//Inode��Ӧ���ļ�����
	int		i_inode_id = 0;			//Inode���
	int		d_mtime = 0;			//������ʱ��
};

/*
SuperBlock���ṹ�壬��Ӧ����SuperBlock����1024B
*/
struct SuperBlock {
	//����Inode
	int s_isize;				//inode������
	int s_ninode;				//����inode����
	int s_inode[100];			//ֱ�ӹ���Ŀ���DiskInode

	//���������̿�
	int s_fsize;				//�����ļ����̿�����
	int s_nfree;				//ֱ�ӹ���Ŀ����̿�����
	int s_free[100];			//ֱ�ӹ���Ŀ����̿�����

	int padding[52];			//���
};

/*BlockGroup��С�鳤�����ڼ�¼�����̿�����Ϣ*/
struct BlockGroup {
	int	g_nblock;				//���̿�����Ŀ
	int	g_block[100];			//���̿��Ӧ�������
};

struct IndexBlock {
	int m_index[BLOCK_SIZE / 4] = { 0 };
};

/*
FileSystem���࣬����SuperBlock�Ĳ������������ʽ��
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
	//�ڴ�������������DiskInode�ṹ
	void Search_FreeInode();
public:
	FileSystem();
	~FileSystem();
	//�����̿���������ͷ�
	int Dalloc();
	void Dfree(int blk_id);
	//����Inode���������ͷ�
	int Ialloc();
	void IFree(int inode_id);

	//��ʽ�����
	void fformat();

	
};

extern FileSystem* g_fs;