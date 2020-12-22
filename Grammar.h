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
	set<char> N;  // множество нетерминальных символов
	set<char> sigma; //множество терминальных символов (алфавит)
	vector<vector<std::string>> P; //множество продукций
	char S; //Стартовый символ

	//Конструктор - считывание грамматики из файла
	Grammar(string path) {
		string s;
		ifstream file(path);
		int i = 1;
		//считываем построчно
		while (getline(file, s)) {
			vector<string> res = split(s, "->"); //вектор одной продукции
			P.push_back(res); //добавляем его во множество продукций
			if (i == 1) { //Заводим стартовый символ
				S = res[0][0];
				i = 0;
			}
			//левая часть продукции - нетерминал
			N.insert(res[0][0]);
			string sigmas = res[1]; // правая часть продукции, с которой мы будем работать, чтобы заполнить сигму
			size_t pos = sigmas.find("eps");
			if (pos != std::string::npos) {
				//sigma.insert('eps');
				res[1].erase(pos, 3);
			}
			for (int i = 0; i < sigmas.length(); i++) {
				if (sigmas[i] != '|' && !isupper(sigmas[i])) //Если очередной символ нетерминал, то вносим его
					sigma.insert(sigmas[i]);
			}
		}
		file.close();
	}

	//Удаление бесполезных символов
	void deleteUselessSymbols() {
		barrenSymbols();
		unreachableSymbols();
	}

	//Печать продукций грамматики
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

	//Печать алфавита грамматики
	void printSigma()
	{
		for (auto it = sigma.begin(); it != sigma.end(); ++it) {
			cout << *it << endl;
		}
	}

	//Измение грамматики после удаления бесплодных символов
	//Если продукция есть в Y, то оставляем, иначе удаляем
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
					string ss = s[i]; //Очередной вывод этой продукции
					size_t pos = ss.find("eps"); //если есть эпсилон, то сразу добавляем во множество
					if (pos != std::string::npos) {
						resS += "eps";
						flag = false;
						goto out;
					}
					for (int ii = 0; ii < ss.length(); ii++) {
						if (isupper(ss[ii]) && Y.find(ss[ii]) == Y.end()) {
							//Если очередной символ терминал, или же находится во множестве
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

	//Изменяем продукции после удаления недостижимых
	//Если продукция есть в Y, то оставляем, иначе удаляем
	//Также контролируем алфавит
	//Еще нам приходится заново перстраивать вектор продукци, т.к. может случится ситуация, когда
	//продкция осталась, но она ведет в ту продукцию, которую мы алгоритмом удалили
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
					string ss = s[i]; //Очередной вывод этой продукции
					size_t pos = ss.find("eps"); //если есть эпсилон, то сразу добавляем во множество
					if (pos != std::string::npos) {
						resS += "eps";
						goto out;
					}
					for (int ii = 0; ii < ss.length(); ii++) {
						if (isupper(ss[ii]) && Y.find(ss[ii]) == Y.end()) {
							//проверяем, если во множестве нет символа, то удаляем продукцию
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

	//удаление бесплодных символов
	void barrenSymbols() {
		set<char> Y; //Нетерминалы, которые по алгоритму достижимы
		bool flag1 = true; //Флаг для контроля, что рассматриваемое правило состоит из терминалов и нетерминалов из множества
		bool flag2 = true; //Флаг для контроля добавили ли мы на очередном проходе по продукциям новый нетерминал в Y
		//Первый проход по продукциям в поисках продукций с обычными нетерминалами
		for (auto it = P.begin(); it != P.end(); ++it) {
			string x = (*it)[0]; //Нетерминал (т.е. левая часть продукции)
			vector<string> s = split((*it)[1], "|"); //Правая часть продукции
			for (int i = 0; i < s.size(); i++) {
				flag1 = true;
				string ss = s[i]; //Очередной вывод этой продукции
				size_t pos = ss.find("eps"); //если есть эпсилон, то сразу добавляем во множество
				if (pos != std::string::npos) {
					goto insert;
				}
				for (int ii = 0; ii < ss.length(); ii++) {
					if (!isupper(ss[ii])) { // ищем выводы, состоящие только из терминалов
						continue;
					}
					else { //иначе в очередном выводе, появились нетерминалы, переходим к следующему выводк
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
			string x = (*it)[0]; //Нетерминал (т.е. левая часть продукции)
			vector<string> s = split((*it)[1], "|"); //Правая часть продукции
			if (Y.find(x[0]) != Y.end()) { //Если нетерминал из левой части уже есть в продукции, то мы можем его заново не просматривать
				goto out2;
			}
			for (int i = 0; i < s.size(); i++) {
				flag1 = true;
				string ss = s[i];
				size_t pos = ss.find("eps"); //если есть эпсилон, то сразу добавляем во множество
				if (pos != std::string::npos) {
					goto out1;
				}
				for (int ii = 0; ii < ss.length(); ii++) {
					if (!isupper(ss[ii]) || Y.find(ss[ii]) != Y.end()) { //Если очередной символ терминал, или же находится во множестве
						continue;
					}
					else {
						flag1 = false;
						goto out1;
						++it;
					}
				}
				if (flag1) { // Если продукция удовлетворяет нашим условиям, то мы ее вносим во можество и переходим к следующей
					flag2 = false;//То есть при новом проходе по продукциям мы что-то добавили во множество=>надо организовать очередной проход
					Y.insert(x[0]);
					goto out2;
				}
			out1: cout << "";
			}
		out2: cout << "";
		}
		//Если мы добавили новый нетерминал, то надо еще раз пройти по продукциям
		if (!flag2)
			goto out3;
		changeAfterBarrenSymbols(Y);
	}

	//Удаление недостижимых символов
	void unreachableSymbols() {
		set<char> Y; //Нетерминалы, которые по алгоритму достижимы
		set<char> Yy; //Терминалы, которые достижимы 
		queue<char> q; //Очередь для того, чтобы обойти все достижимые нетерминалы
		Y.insert(S);
		q.push(S);
		while (!q.empty()) {
			bool flag = true; //Флаг для проверки, что мы рассматриваем левую или правую часть продукции, true - левая часть
			for (auto it = P.begin(); it != P.end(); ++it) {
				for (int i = 0; i < (*it).size(); i++) {
				go:
					string ss = (*it)[i];
					if (ss[0] == q.front() && flag == true) { //Если символ совпадает с символом в очереди, то разбирать будем его правую часть продукции
						flag = false;
						i++;
						goto go;
					}
					else if (flag == false) {
						size_t pos = ss.find("eps"); //удалим просто все эпсилон продукции, чтоб не мешали
						if (pos != std::string::npos) {
							ss.erase(pos, 3);
						}
						for (int i = 0; i < ss.length(); i++) { //проходим по правой части нужной нам продукции
							if (ss[i] != '|' && isupper(ss[i]) && Y.find(ss[i]) == Y.end()) { //Если символ-нетерминал и его нет во множестве нетерминалов, 
								//то добавляем его туда и в очередь
								Y.insert(ss[i]);
								q.push(ss[i]);
							}
							else if (ss[i] != '|' && islower(ss[i])) {//если символ терминал, то добавляем его во множество терминалов
								Yy.insert(ss[i]);
							}
						}
						flag = true; //закончили с правой частью
						goto out; // переходим к следующему символу в очереди
					}
				}
			}
		out:
			q.pop();//убираем из очереди обработанный символ
		}
		changePAfterUnreachable(Y);
		sigma = Yy;
	}
};