#ifndef MYFUNCTIONS_HPP
#define MYFUNCTIONS_HPP

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdarg>

using namespace std;

unsigned int 	nofwords(string& str) {
	int out=0;
	for(unsigned int i=1; i<str.length(); i++)
		if(str[i]==' ' && str[i-1]!=' ')
			out++;
	if(str.length()!=0 && str[str.length()-1]!=' ') out++;
	return out;
}

string 			nthword(string& str, unsigned int n) { // "n"-edik szó egy "str" stringben, memóriaterületet foglal le.
		if(n<=nofwords(str)) {
			int x=0;
			for(unsigned int j=0; j<n-1; j++) {
				for(int i=x; str[i]!=' '; i++) x++;
				x++;
			}
			string szo;
			for(unsigned int i=x; str[i]!=' ' && i!=str.length(); i++) szo+=str[i];
			return szo;
		} else throw ("nthword: nincs eleg szo a stringben");
}

int numberfromhere(string& str, unsigned int& n) {
	string seged;
	while(isdigit(str[n]) && n!=str.length()) {
		seged+=str[n];
		n++;
	}
	return atoi(seged.c_str());
}

bool anytrue(int n, ... ) {
  int i;
  bool val;
  va_list vl;
  va_start(vl,n);
  for (i=0;i<n;i++) {
    val=va_arg(vl,int);
    if(val) return true;
    cout << val << endl;
  }
  va_end(vl);
  return false;
}

#endif
