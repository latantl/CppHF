#include <string>
#include <iostream>
#include <iomanip>
#include "memtrace.h"
#include "neumann.hpp"
using namespace neumann;

int main() {
	string str;
	cin >> str;
	memory* neumann=memoryfromfile(str);
	cout << "memoriaterulet mereete: " << neumann->getSMEM() << endl;
	cout << "konstansoknak foglalt terulet: " << neumann->getSKONST() << endl;
	cout << "konstansok szama: " << neumann->getNKONST() << endl;
	cout << "utasitaspoinetereknek foglalt terulet: " << neumann->getSINST() << endl;
	cout << "utasitaspointerek szama: " << neumann->getNINST() << endl << endl;
	cout << "konstansok erteke es cime:" << endl;
	for(int i=1; i<=neumann->getNKONST(); i++) {
		cout << i << ".: " << *(neumann->konst(i)) << "   " << neumann->konst(i) << endl;
	}
	cout << endl;
	cout << "memoriateruletek cime:" << endl;
	for(int i=1; i<=neumann->getSMEM(); i++) {
		cout << setw(2) << i << "  " << neumann->mem(i) << endl;
	}
	cout << endl;
	cout << "UTASITASOK:" << endl << endl;
	for(int i=1; i<=neumann->getNINST(); i++) neumann->inst(i)->testwrite(std::cout);
	return 0;
}

