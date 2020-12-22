#include "Grammar.h"

/* �������� ���������, ������� ��������� �� ����� �������� ����������, ������� �� ��� ����������� 
������� � ������� ����� � ��������� ����� ����������. ���������� ��������� ����� ���: 
�S->i(Adf)3a|aDe A->Sa|4a|\e D-> ...� 
������������, � ���� ������ ������� � ����� ������ � ������� ������������ ��� eps(� ���� ���� �������� � ����� ����)
*/

int main() {
	Grammar gr("gr.txt");
	gr.printP();
	gr.unreachableSymbols();
	gr.printP();
	Grammar gr1("gr1.txt");
	gr1.printP();
	gr1.barrenSymbols();
	gr1.printP();
	Grammar gr2("gr2.txt");
	gr2.printP();
	gr2.deleteUselessSymbols();
	gr2.printP();
	system("pause");
	return 0;
}