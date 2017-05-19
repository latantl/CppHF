#include "memtrace.h"
#include <string>
#include <iostream>
#include "rlutil.h"
#include "neumann.hpp"
#include "modelling.hpp"



using namespace std;
using namespace rlutil;
using namespace neumann;

int main() {
	string filename;
	cout << "file: ";
	cin >> filename;
	try {
		memory* neumann=memoryfromfile(filename);
		hidecursor();
		modelling(filename, neumann);
		delete neumann;
	} catch(const char* e) {
		cout << e;
	}
	return 0;
}
