#include "Grammar.h"

/* Ќапишите программу, котора€ считывает из файла описание грамматики, удал€ет из нее бесполезные 
символы и выводит текст с описанием новой грамматики. √рамматику описывать можно так: 
УS->i(Adf)3a|aDe A->Sa|4a|\e D-> ...Ф 
≈динственное, у мен€ каждое правило с новой строки и эпсилон обозначаетс€ как eps(с этим были проблемы в самом коде)
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