#pragma once
#include <iostream>
#include <cstdlib> 
#include <set>
#include <vector>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
using namespace std;

vector<string> split(const string& str, const string& delim)
{
	vector<string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == string::npos) pos = str.length();
		string token = str.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}

class Grammar {
public:
	set<char> N;  // ��������� �������������� ��������
	set<char> sigma; //��������� ������������ �������� (�������)
	vector<vector<std::string>> P; //��������� ���������
	char S; //��������� ������

	//����������� - ���������� ���������� �� �����
	Grammar(string path) {
		string s;
		ifstream file(path);
		int i = 1;
		//��������� ���������
		while (getline(file, s)) {
			vector<string> res = split(s, "->"); //������ ����� ���������
			P.push_back(res); //��������� ��� �� ��������� ���������
			if (i == 1) { //������� ��������� ������
				S = res[0][0];
				i = 0;
			}
			//����� ����� ��������� - ����������
			N.insert(res[0][0]);
			string sigmas = res[1]; // ������ ����� ���������, � ������� �� ����� ��������, ����� ��������� �����
			size_t pos = sigmas.find("eps");
			if (pos != std::string::npos) {
				//sigma.insert('eps');
				res[1].erase(pos, 3);
			}
			for (int i = 0; i < sigmas.length(); i++) {
				if (sigmas[i] != '|' && !isupper(sigmas[i])) //���� ��������� ������ ����������, �� ������ ���
					sigma.insert(sigmas[i]);
			}
		}
		file.close();
	}

	//�������� ����������� ��������
	void deleteUselessSymbols() {
		barrenSymbols();
		unreachableSymbols();
	}

	//������ ��������� ����������
	void printP()
	{
		bool flag = true;
		for (auto it = P.begin(); it != P.end(); ++it) {
			for (vector<string>::iterator it2 = (*it).begin(); it2 != (*it).end(); ++it2) {
				if (flag) {
					std::cout << *it2 << " -> ";
					flag = false;
				}
				else cout << *it2;
			}
			std::cout << endl;
			flag = true;
		}
		cout << endl;
	}

	//������ �������� ����������
	void printSigma()
	{
		for (auto it = sigma.begin(); it != sigma.end(); ++it) {
			cout << *it << endl;
		}
	}

	//������� ���������� ����� �������� ���������� ��������
	//���� ��������� ���� � Y, �� ���������, ����� �������
	void changeAfterBarrenSymbols(set<char> Y) {
		vector<vector<string>> res;
		for (auto it = P.begin(); it != P.end(); ++it) {
			string ss = (*it)[0];
			vector<string> resSS;
			resSS.push_back((*it)[0]);
			if (Y.find(ss[0]) != Y.end()) {
				string resS;
				vector<string> s = split((*it)[1], "|");
				for (int i = 0; i < s.size(); i++) {
					bool flag = true;
					string ss = s[i]; //��������� ����� ���� ���������
					size_t pos = ss.find("eps"); //���� ���� �������, �� ����� ��������� �� ���������
					if (pos != std::string::npos) {
						resS += "eps";
						flag = false;
						goto out;
					}
					for (int ii = 0; ii < ss.length(); ii++) {
						if (isupper(ss[ii]) && Y.find(ss[ii]) == Y.end()) {
							//���� ��������� ������ ��������, ��� �� ��������� �� ���������
							flag = false;
							goto out1;
							++it;
						}
					}
				out: cout << "";
					if (flag) resS += ss;
					if (i < s.size() - 1 || (i > 0 && i < s.size() - 1)) resS += "|";
				out1: cout << "";
				}
				resSS.push_back(resS);
				res.push_back(resSS);
			}
		}
		P = res;
	}

	//�������� ��������� ����� �������� ������������
	//���� ��������� ���� � Y, �� ���������, ����� �������
	//����� ������������ �������
	//��� ��� ���������� ������ ������������ ������ ��������, �.�. ����� �������� ��������, �����
	//�������� ��������, �� ��� ����� � �� ���������, ������� �� ���������� �������
	void changePAfterUnreachable(set<char> Y) {
		vector<vector<string>> res;
		set<char> resSigma;
		for (auto it = P.begin(); it != P.end(); ++it) {
			string ss = (*it)[0];
			vector<string> resSS;
			resSS.push_back((*it)[0]);
			if (Y.find(ss[0]) != Y.end()) {
				string resS;
				vector<string> s = split((*it)[1], "|"); 
				for (int i = 0; i < s.size(); i++) {
					bool flag = true;
					string ss = s[i]; //��������� ����� ���� ���������
					size_t pos = ss.find("eps"); //���� ���� �������, �� ����� ��������� �� ���������
					if (pos != std::string::npos) {
						resS += "eps";
						goto out;
					}
					for (int ii = 0; ii < ss.length(); ii++) {
						if (isupper(ss[ii]) && Y.find(ss[ii]) == Y.end()) {
							//���������, ���� �� ��������� ��� �������, �� ������� ���������
							flag = false;
							goto out;
							++it;
						}
						else if (!isupper(ss[ii])) {
							resSigma.insert(ss[ii]);
						}
					}
				out: cout << "";
					if (flag) resS += ss;
					if (i < s.size()-1 || (i > 0 && i < s.size()-1)) resS += "|";
				}
				resSS.push_back(resS);
				res.push_back(resSS);
			}
		}
		P = res;
		sigma = resSigma;
	}

	//�������� ���������� ��������
	void barrenSymbols() {
		set<char> Y; //�����������, ������� �� ��������� ���������
		bool flag1 = true; //���� ��� ��������, ��� ��������������� ������� ������� �� ���������� � ������������ �� ���������
		bool flag2 = true; //���� ��� �������� �������� �� �� �� ��������� ������� �� ���������� ����� ���������� � Y
		//������ ������ �� ���������� � ������� ��������� � �������� �������������
		for (auto it = P.begin(); it != P.end(); ++it) {
			string x = (*it)[0]; //���������� (�.�. ����� ����� ���������)
			vector<string> s = split((*it)[1], "|"); //������ ����� ���������
			for (int i = 0; i < s.size(); i++) {
				flag1 = true;
				string ss = s[i]; //��������� ����� ���� ���������
				size_t pos = ss.find("eps"); //���� ���� �������, �� ����� ��������� �� ���������
				if (pos != std::string::npos) {
					goto insert;
				}
				for (int ii = 0; ii < ss.length(); ii++) {
					if (!isupper(ss[ii])) { // ���� ������, ��������� ������ �� ����������
						continue;
					}
					else { //����� � ��������� ������, ��������� �����������, ��������� � ���������� ������
						flag1 = false;
						goto out;
						++it;
					}
				}
				if (flag1) {
				insert:
					Y.insert(x[0]);
				}
			out: cout << "";
			}
		}
		flag1 = true;
	out3: flag2 = true;
		for (auto it = P.begin(); it != P.end(); ++it) {
			string x = (*it)[0]; //���������� (�.�. ����� ����� ���������)
			vector<string> s = split((*it)[1], "|"); //������ ����� ���������
			if (Y.find(x[0]) != Y.end()) { //���� ���������� �� ����� ����� ��� ���� � ���������, �� �� ����� ��� ������ �� �������������
				goto out2;
			}
			for (int i = 0; i < s.size(); i++) {
				flag1 = true;
				string ss = s[i];
				size_t pos = ss.find("eps"); //���� ���� �������, �� ����� ��������� �� ���������
				if (pos != std::string::npos) {
					goto out1;
				}
				for (int ii = 0; ii < ss.length(); ii++) {
					if (!isupper(ss[ii]) || Y.find(ss[ii]) != Y.end()) { //���� ��������� ������ ��������, ��� �� ��������� �� ���������
						continue;
					}
					else {
						flag1 = false;
						goto out1;
						++it;
					}
				}
				if (flag1) { // ���� ��������� ������������� ����� ��������, �� �� �� ������ �� �������� � ��������� � ���������
					flag2 = false;//�� ���� ��� ����� ������� �� ���������� �� ���-�� �������� �� ���������=>���� ������������ ��������� ������
					Y.insert(x[0]);
					goto out2;
				}
			out1: cout << "";
			}
		out2: cout << "";
		}
		//���� �� �������� ����� ����������, �� ���� ��� ��� ������ �� ����������
		if (!flag2)
			goto out3;
		changeAfterBarrenSymbols(Y);
	}

	//�������� ������������ ��������
	void unreachableSymbols() {
		set<char> Y; //�����������, ������� �� ��������� ���������
		set<char> Yy; //���������, ������� ��������� 
		queue<char> q; //������� ��� ����, ����� ������ ��� ���������� �����������
		Y.insert(S);
		q.push(S);
		while (!q.empty()) {
			bool flag = true; //���� ��� ��������, ��� �� ������������� ����� ��� ������ ����� ���������, true - ����� �����
			for (auto it = P.begin(); it != P.end(); ++it) {
				for (int i = 0; i < (*it).size(); i++) {
				go:
					string ss = (*it)[i];
					if (ss[0] == q.front() && flag == true) { //���� ������ ��������� � �������� � �������, �� ��������� ����� ��� ������ ����� ���������
						flag = false;
						i++;
						goto go;
					}
					else if (flag == false) {
						size_t pos = ss.find("eps"); //������ ������ ��� ������� ���������, ���� �� ������
						if (pos != std::string::npos) {
							ss.erase(pos, 3);
						}
						for (int i = 0; i < ss.length(); i++) { //�������� �� ������ ����� ������ ��� ���������
							if (ss[i] != '|' && isupper(ss[i]) && Y.find(ss[i]) == Y.end()) { //���� ������-���������� � ��� ��� �� ��������� ������������, 
								//�� ��������� ��� ���� � � �������
								Y.insert(ss[i]);
								q.push(ss[i]);
							}
							else if (ss[i] != '|' && islower(ss[i])) {//���� ������ ��������, �� ��������� ��� �� ��������� ����������
								Yy.insert(ss[i]);
							}
						}
						flag = true; //��������� � ������ ������
						goto out; // ��������� � ���������� ������� � �������
					}
				}
			}
		out:
			q.pop();//������� �� ������� ������������ ������
		}
		changePAfterUnreachable(Y);
		sigma = Yy;
	}
};