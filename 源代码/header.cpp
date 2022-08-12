#include"header.h"
//���ַ�sep�ָ��ַ���
vector<string> split(string str, char sep) {
	vector<string> res;
	string temp;
	for (u_int i = 0; i < str.length();i++) {
		char c = str[i];
		if (c != sep) {
			temp += c;
		}
		else if (temp != "") {
			res.push_back(temp);
			temp = "";
		}
	}
	if (temp != "")
		res.push_back(temp);
	return res;
}

//ȡ·�����ϸ���Ŀ¼
vector<string> split_fp(string fp) {
	return split(fp, '/');
}

//ȡ���ļ�����Ŀ¼�����һ�
string get_filename(string fp) {
	int i = 0;
	if (fp.back() == '/') return "";
	//��Χ��[���һ��/����һ��,ĩβ]
	//����/usr/Jerry���򷵻�Jerry
	for (i = fp.length()-1; i >= 0 && fp[i] != '/'; i--){
		continue;
	}
	return i == -1 ? fp : fp.substr(i + 1);
}

//ƴ���ļ���
string join(string fp1, string fp2) {
	// /mnt/w1951444 + /dev = /mnt/w1951444/dev
	if (fp1.length() == 0) return fp2;
	if (fp2.length() == 0) return fp1;
	if (fp1.back() != '/' && *fp2.begin() != '/')
		return fp1 + "/" + fp2;
	return fp1 + fp2;
}

//��ȡ���Ե�ַ
string abspath(string fp) {
	vector<string> abs;
	vector<string> tokens = split_fp(fp);
	for (auto& str : tokens) {
		//�����ֶ����..��˵�������ˣ���ʱ����
		if (str == ".") continue;
		else if (str == ".." && abs.size() != 0) {
			abs.pop_back();
		}
		else {
			abs.push_back(str);
		}
	}
	string res = "";
	for (auto& str : abs) {
		res += "/" + str;
	}
	if (res == "") res = "/";
	return res;
}

//�ӱ��ض�ȡ�ļ�
string readfile(string fp) {
	string res = "";
	string temp;
	ifstream in(fp, ios::in | ios::binary);
	if (!in.is_open()) {
		return res;
	}
	while (!in.eof()) {
		char c = in.get();
		res += c;
	}
	return res;
}

void writefile(string fp,string str) {
	ofstream out(fp, ios::out | ios::binary);
	if (!out.is_open()) {
		return;
	}
	out.seekp(0, ios::beg);
	out.write(str.c_str(), str.length());
}

string CmdInput() {
	string res;
	string temp;
	char c;
	while (!cin.eof()) {
		getline(cin, temp);
		if (temp == "#") break;
		res += (temp + "\n");
	}
	//if (res.back() == '\n') res.pop_back();
	
	return res;
}

int to_int(string str, int base) {
	int res = 0;
	try { res = stoi(str, nullptr, base); }
	catch (std::invalid_argument&) { res = 0; }
	catch (std::out_of_range&) { res = 0; }
	return res;
}

string get_father(string fp) {
	// /bin/test -> /bin
	// /test -> /
	// / -> ""
	// test -> .
	// 
	//�ȱ�֤Ŀ¼��./��/��ͷ����������Ŀ¼�Ļ�
	if (fp == "") return fp;
	if (fp[0] != '/')
		fp = "./" + fp;
	else
		fp = "/" + fp;
	for (int i = fp.length() - 1; i >= 0; i--) {
		if (fp[i] == '/') {
			return fp.substr(0, i);
		}
	}
	return "";
	//return fp + "/..";
}