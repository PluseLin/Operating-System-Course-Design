#pragma once
#include"header.h"

extern const char* disk_fp;

/*
Buffer，结构体，针对于盘块，其实就是将char[]包装了一下
*/
//struct Buffer {
//	char buf[BLOCK_SIZE] = { 0 };
//};

struct Block {
	char	data[BLOCK_SIZE] = { 0 };
};

/*
IO_Buffer，结构体，用于高速读写缓存，使用LRU算法。
由于Ljyux实现跟Unix V6++的大环境不同，因此这里算法略有差异
*/
struct IO_Buffer {
	char	m_buf[BLOCK_SIZE]={0};	//缓存块	
	int		blk_id = -1;			//对应blk号
	int		m_nutime = 0;			//上次未被使用时间
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
DiskDevice，类，封装了对磁盘文件的读写
提供读写函数接口，参数为读写盘块，偏移量，读写大小，读写内容
*/
class DiskDevice{
private:
	fstream			d_fs;					//文件读写类
	IO_Buffer		m_buffer[BUFFER_NUM];	//缓存块
private:
	//文件不存在的相关操作
	//判断文件是否存在
	bool file_exist(const char* fp);	

public:
	int	d_newfile = 0;	//仅初始化用
public:
	DiskDevice(const char* fp=disk_fp);
	~DiskDevice();
	IO_Buffer* Bget(int blk_id,int read);
	//写入空文件
	void create_null_file(const char* fp, int blk_num);
	//清除缓存
	void clearbuffer();
	//封装了读写操作
	template<class T>
	void write(int blk_id, int offset, T* t, int size = sizeof(T));

	template<class T>
	void read(int blk_id, int offset, T* t, int size = sizeof(T));

};

template<class T>
void DiskDevice::write(int blk_id, int offset, T* t, int size) {
	//d_fs.seekp(blk_id * BLOCK_SIZE + offset, ios::beg);
	//d_fs.write(reinterpret_cast<char*>(t), size);
	//将offset化为0-512范围内

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
		//盘块写的时候，如果不是从开头写满一个盘块，则需要先读出来
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
	//将offset化为0-512范围内

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
