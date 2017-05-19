#include <string>
#include <iostream>
#include "memtrace.h"
#include "neumann.hpp"
#include "modelling.hpp"

using namespace std;
using namespace neumann;

int main() {
	string filename;
	cin >> filename;
	try {
		memory* neumann=memoryfromfile(filename);
		stmodelling(neumann);
		delete neumann;
	} catch(const char* e) {
		cout << e;
	}
	return 0;
}
