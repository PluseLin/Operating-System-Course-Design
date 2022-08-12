#pragma once
#include"header.h"

extern const char* disk_fp;

/*
Buffer���ṹ�壬������̿飬��ʵ���ǽ�char[]��װ��һ��
*/
//struct Buffer {
//	char buf[BLOCK_SIZE] = { 0 };
//};

struct Block {
	char	data[BLOCK_SIZE] = { 0 };
};

/*
IO_Buffer���ṹ�壬���ڸ��ٶ�д���棬ʹ��LRU�㷨��
����Ljyuxʵ�ָ�Unix V6++�Ĵ󻷾���ͬ����������㷨���в���
*/
struct IO_Buffer {
	char	m_buf[BLOCK_SIZE]={0};	//�����	
	int		blk_id = -1;			//��Ӧblk��
	int		m_nutime = 0;			//�ϴ�δ��ʹ��ʱ��
};

class BufferManager {
public:
	IO_Buffer	m_buffer[BUFFER_NUM];
public:
	BufferManager();
	~BufferManager();
	//Buffer* Bget(int blk_id);
};

/*
DiskDevice���࣬��װ�˶Դ����ļ��Ķ�д
�ṩ��д�����ӿڣ�����Ϊ��д�̿飬ƫ��������д��С����д����
*/
class DiskDevice{
private:
	fstream			d_fs;					//�ļ���д��
	IO_Buffer		m_buffer[BUFFER_NUM];	//�����
private:
	//�ļ������ڵ���ز���
	//�ж��ļ��Ƿ����
	bool file_exist(const char* fp);	

public:
	int	d_newfile = 0;	//����ʼ����
public:
	DiskDevice(const char* fp=disk_fp);
	~DiskDevice();
	IO_Buffer* Bget(int blk_id,int read);
	//д����ļ�
	void create_null_file(const char* fp, int blk_num);
	//�������
	void clearbuffer();
	//��װ�˶�д����
	template<class T>
	void write(int blk_id, int offset, T* t, int size = sizeof(T));

	template<class T>
	void read(int blk_id, int offset, T* t, int size = sizeof(T));

};

template<class T>
void DiskDevice::write(int blk_id, int offset, T* t, int size) {
	//d_fs.seekp(blk_id * BLOCK_SIZE + offset, ios::beg);
	//d_fs.write(reinterpret_cast<char*>(t), size);
	//��offset��Ϊ0-512��Χ��

	blk_id += offset / BLOCK_SIZE;
	offset %= BLOCK_SIZE;

	char* ptr = new char[size + 1],
		* q = ptr;
	memcpy(ptr, reinterpret_cast<char*>(t), size);
	while (size > 0) {
		/*IO_Buffer* buf = Bget(blk_id, 1);*/
		//cout << blk_id << " " << buf->blk_id << endl;
		int w_offset = offset % BLOCK_SIZE;
		int wsize = min(size, BLOCK_SIZE - w_offset);
		//�̿�д��ʱ��������Ǵӿ�ͷд��һ���̿飬����Ҫ�ȶ�����
		IO_Buffer* buf = (w_offset != 0 || wsize != BLOCK_SIZE) ?
			Bget(blk_id, 1) : Bget(blk_id, 0);
		memcpy(buf->m_buf + w_offset, q, wsize);
		q += wsize;
		size -= wsize;
		offset += wsize;
		blk_id++;
	}
	delete[] ptr;
}
template<class T>
void DiskDevice::read(int blk_id, int offset, T* t, int size) {
	//d_fs.seekg(blk_id * BLOCK_SIZE + offset, ios::beg);
	//d_fs.read(reinterpret_cast<char*>(t), size);
	//��offset��Ϊ0-512��Χ��

	blk_id += offset / BLOCK_SIZE;
	offset %= BLOCK_SIZE;
	int size1 = size;
	char* ptr = new char[size+1],
		* q = ptr;

	while (size > 0) {
		int r_offset = offset % BLOCK_SIZE;
		int rsize = min(size, BLOCK_SIZE - r_offset);
		IO_Buffer* buf = Bget(blk_id, 1);
		memcpy(q, buf->m_buf + r_offset, rsize);
		q += rsize;
		size -= rsize;
		offset += rsize;
		blk_id++;
	}
	memcpy(reinterpret_cast<char*>(t), ptr, size1);
	delete[]ptr;
}

extern DiskDevice* g_Disk;
