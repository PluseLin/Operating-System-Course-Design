#include "DiskDevice.h"



BufferManager::BufferManager() {

}

BufferManager::~BufferManager() {

}

IO_Buffer*	DiskDevice::Bget(int blk_id,int read) {
	//检查有无命中
	//检查时，未命中者nutime均加，命中者先加1，后改为0
	int target = BUFFER_NUM;
	int nutime_maxer = 0;
	int first_free = -1;
	int i = 0;
	for (i = 0; i < BUFFER_NUM; i++) {
		if (m_buffer[i].blk_id == blk_id) {
			target = i;
		}
		nutime_maxer = (m_buffer[nutime_maxer].m_nutime < m_buffer[i].m_nutime) ? 
			i : nutime_maxer;
		if (m_buffer[i].blk_id != -1) 
			m_buffer[i].m_nutime++;
		else if (first_free == -1) 
			first_free = i;
	}
	//命中了，返回此buffer
	if (target < BUFFER_NUM) {
		m_buffer[target].m_nutime = 0;
		return (m_buffer + target);
	}
	//没命中，尝试分配
	else {
		//有空闲，赋予空闲者，否则赋予未使用时间最大者
		target = (first_free != -1) ? first_free : nutime_maxer;
		//无论读写模式，都要把脏块写回去
		if (m_buffer[target].blk_id != -1) {
			d_fs.seekp(m_buffer[target].blk_id * BLOCK_SIZE, ios::beg);
			d_fs.write(m_buffer[target].m_buf, BLOCK_SIZE);
		}
		//读模式下，要读出来
		if (read) {
			d_fs.seekg(blk_id * BLOCK_SIZE, ios::beg);
			d_fs.read(m_buffer[target].m_buf, BLOCK_SIZE);
		}
		//修改缓存块信息
		m_buffer[target].blk_id = blk_id;
		m_buffer[target].m_nutime = 0;
		return (m_buffer+target);
	}
	//防止编译warning
	return nullptr;
}

DiskDevice::DiskDevice(const char* fp) {
	if (!file_exist(fp)) {
		create_null_file(fp, DISK_BLOCK_NUM);
		d_newfile = 1;
	}
	d_fs.open(fp, ios::in | ios::out | ios::binary);
}

void DiskDevice::clearbuffer() {
	for (int i = 0; i < BUFFER_NUM; i++) {
		memset(m_buffer + i, 0, sizeof(IO_Buffer));
		m_buffer[i].blk_id = -1;
	}
}

DiskDevice::~DiskDevice() {
	//缓存块全部写回磁盘
	for (int i = 0; i < BUFFER_NUM; i++) {
		if (m_buffer[i].blk_id != -1) {
			d_fs.seekp(m_buffer[i].blk_id * BLOCK_SIZE, ios::beg);
			d_fs.write(m_buffer[i].m_buf, BLOCK_SIZE);
		}
	}
	d_fs.close();
}

//文件不存在的相关操作
bool DiskDevice::file_exist(const char* fp) {
	ifstream in(fp);
	return in.good();
}

void DiskDevice::create_null_file(const char* fp, int blk_num) {
	ofstream out(fp);
	Block buf;
	out.seekp(0, ios::beg);
	for (int i = 0; i < blk_num; i++) {
		out.write(buf.data, BLOCK_SIZE);
	}
	out.close();
}

