#include "DiskDevice.h"



BufferManager::BufferManager() {

}

BufferManager::~BufferManager() {

}

IO_Buffer*	DiskDevice::Bget(int blk_id,int read) {
	//�����������
	//���ʱ��δ������nutime���ӣ��������ȼ�1�����Ϊ0
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
	//�����ˣ����ش�buffer
	if (target < BUFFER_NUM) {
		m_buffer[target].m_nutime = 0;
		return (m_buffer + target);
	}
	//û���У����Է���
	else {
		//�п��У���������ߣ�������δʹ��ʱ�������
		target = (first_free != -1) ? first_free : nutime_maxer;
		//���۶�дģʽ����Ҫ�����д��ȥ
		if (m_buffer[target].blk_id != -1) {
			d_fs.seekp(m_buffer[target].blk_id * BLOCK_SIZE, ios::beg);
			d_fs.write(m_buffer[target].m_buf, BLOCK_SIZE);
		}
		//��ģʽ�£�Ҫ������
		if (read) {
			d_fs.seekg(blk_id * BLOCK_SIZE, ios::beg);
			d_fs.read(m_buffer[target].m_buf, BLOCK_SIZE);
		}
		//�޸Ļ������Ϣ
		m_buffer[target].blk_id = blk_id;
		m_buffer[target].m_nutime = 0;
		return (m_buffer+target);
	}
	//��ֹ����warning
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
	//�����ȫ��д�ش���
	for (int i = 0; i < BUFFER_NUM; i++) {
		if (m_buffer[i].blk_id != -1) {
			d_fs.seekp(m_buffer[i].blk_id * BLOCK_SIZE, ios::beg);
			d_fs.write(m_buffer[i].m_buf, BLOCK_SIZE);
		}
	}
	d_fs.close();
}

//�ļ������ڵ���ز���
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

