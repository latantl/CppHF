#ifndef NEUMANN_HPP
#define NEUMANN_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "statvec.hpp"
#include "myfunctions.hpp"
using namespace std;

namespace neumann {

istream& getline(istream& is, string& str) {
std::getline(is, str);
if (str.size() && *str.rbegin() == '\r')
str.erase(str.size()-1);
return is;
}
}

namespace neumann {

struct token {
	bool cond;
	int counter;
	token() :cond(false), counter(0) {}
};

class instruction {
protected:
	string row;
	int chmem;
public:
	instruction(const string& row, int cmem) :row(row), chmem(cmem) {}
	virtual ~instruction() {}
	virtual void run(token& tok)=0;
	string getRow() const { return row; }
	int changedmem() const { return chmem; }
	virtual void testwrite(ostream& os)=0;
};

class memory {
	token tok;
	statvec<int> MEM;
	statvec<instruction*> INST;
	statvec<int> KONST;
public:
	memory(unsigned int op1, unsigned int op2, unsigned int op3) {
		INST.editsize(op1);
		MEM.editsize(op2);
		KONST.editsize(op3);
		for(int i=0; i<INST.getS(); i++) INST[i]=NULL;
		for(int i=0; i<MEM.getS(); i++) MEM[i]=0;
		tok.counter=0;
	}
	~memory() {
		for(int i=0; i<INST.getN(); i++) delete INST[i];
	}
	int getSMEM() { return MEM.getS(); }
	int getSINST() { return INST.getS(); }
	int getNINST() { return INST.getN(); }
	int getSKONST() { return KONST.getS(); }
	int getNKONST() { return KONST.getN(); }
	int* mem(const unsigned int index) { return &MEM[index-1]; }
	int* mem(const unsigned int index) const { return &MEM[index-1]; }
	instruction* inst(const unsigned int index) { return INST[index-1]; }
	instruction* inst(const unsigned int index) const { return INST[index-1]; }
	int* konst(const unsigned int index) { return &KONST[index-1]; }
	int* konst(const unsigned int index) const { return &KONST[index-1]; }
	void addinst(instruction* op) { INST.add(op);	}
	int* addconst(int op) { KONST.add(op); return &KONST.last(); }
	bool ended() const { return tok.counter>=INST.getN(); }
	bool next() {
		if(!this->ended()) {
			INST[tok.counter]->run(tok);
			return true;
		}
		return false;
	}
	int tokenpos() { return tok.counter+1; }
	bool tokenbool() { return tok.cond; }
	
	void reset() {
		tok.counter=0;
		for(int i=0; i<MEM.getS(); i++) MEM[i]=0;
	}
	bool constfull() const { return KONST.full(); }
	bool instempty() const { return INST.empty(); } // üres-e az utasításpointerek tömbje
	bool instfull() const { return INST.full(); } // tele van-e az utasításpointerek tömbje
};

class mov :public instruction {
	int* to;
	int* from;
public:
	static string instname;
	virtual void testwrite(ostream& os) { os << row << "; to: " << to << "; from: " << from << "; chmem:" << chmem << endl; }
	mov(int* op1, int* op2, const string& row, int chmem) :instruction(row,chmem) { to=op1; from=op2; }
	virtual void run(token& tok) { *to=*from; tok.counter++; }
};
string mov::instname="MOV";

class add :public instruction {
	int* target;
	int* value;
public:
	static string instname;
	virtual void testwrite(ostream& os) { os << row << "; target: " << target << "; value: " << value << "; chmem:" << chmem << endl; }
	add(int* op1, int* op2, const string& row, int chmem) :instruction(row,chmem) { target=op1; value=op2; }
	virtual void run(token& tok) { *target+=*value; tok.counter++; }
};
string add::instname="ADD";

class cmp :public instruction {
	int* comp1;
	int* comp2;
public:
	static string instname;
	virtual void testwrite(ostream& os) { os << row << "; comp1: " << comp1 << "; comp2: " << comp2 << "; chmem:" << chmem << endl; }
	cmp(int* op1, int* op2, const string& row, int chmem) :instruction(row,0) { comp1=op1; comp2=op2; }
	virtual void run(token& tok) { tok.cond=(*comp1==*comp2); tok.counter++; }
};
string cmp::instname="CMP";

class jmc :public instruction {
	int destination;
public:
	static string instname;
	virtual void testwrite(ostream& os) { os << row << "; destination: " << destination << "; chmem:" << chmem << endl; }
	jmc(int op1, const string& row) :instruction(row,0) { destination=op1-1; }
	virtual void run(token& tok) { if(tok.cond) tok.counter=destination; else tok.counter++; }
};
string jmc::instname="JMC";

class jmp: public instruction {
	int destination;
public:
	static string instname;
	virtual void testwrite(ostream& os) { os << row << "; destination: " << destination << "; chmem:" << chmem << endl; }
	jmp(int op1, const string& row) :instruction(row,0) { destination=op1-1; }
	virtual void run(token& tok) { tok.counter=destination; }
};
string jmp::instname="JMP";

namespace beforebuild { // uj utasitasosztalynal a checkalg() fgv-t ki kell egesziteni
	template<class C>
	bool check_1(string& row, int& nm0, int& nc0) { // 1. OP: M[posint] 2. OP: M[posint]/posint
	//mivel a mov,add,cmp utasítások leírásának formája ugyanaz, ezért elég ugyanazzal a függvénnyel vizsgálni
		if(row[0]!=C::instname[0] || row[1]!=C::instname[1] || row[2]!=C::instname[2] || row[3]!=' ' ) return false;
		unsigned int x=4; //vizsgáló kurzor helye
		if(row[x]=='M' && row[x+1]=='[') {
			x+=2;
			nm0=numberfromhere(row,x);
			if(nm0==0) return false;
			if(row[x]==']') x++;
			else return false;
		} else return false;
		//elsõ operandus helyessége kiértékelve
		if(row[x]==' ') x++;
		else return false;
		//operandusokat elválasztó szóköz ottléte meghatározva
		if(row[x]=='M' && row[x+1]=='[') {
			x+=2;
			int i=numberfromhere(row,x);
			if(i==0) return false;
			if(i>nm0) nm0=i;
			if(row[x]==']') x++;
			else return false;
		} else if(isdigit(row[x])) {
			while(isdigit(row[x]) && x!=row.length()) x++;
			if(x!=row.length()) return false;
			//megvizsgálva, hogy nincsenek-e a sor végén további karakterek
			nc0++;
		} else return false;
		//második operandus helyessége kiértékelve
		return true;
	}
	
	template<class C>
	bool check_2(string& row, int& nm0, int& nc0) { // egyetlen OP: posint
			//mivel a mov,add,cmp utasítások leírásának formája ugyanaz, ezért elég ugyanazzal a függvénnyel vizsgálni
		if(row[0]!=C::instname[0] || row[1]!=C::instname[1] || row[2]!=C::instname[2] || row[3]!=' ' ) return false;
		unsigned int x=4; //vizsgáló kurzor helye
		while(isdigit(row[x]) && x!=row.length()) x++;
		if(x!=row.length()) return false;
		return true;
	}
	
	bool checkrow(string& row, int& nm, int& nc) { //tartalmazza: utasitastipusonkent megfelelo formaju-e. Ha legalabb egy utasitastipusnak megfelel a formaja, akkor true
		if(check_1<mov>(row,nm,nc)) return true;
		if(check_1<add>(row,nm,nc)) return true;
		if(check_1<cmp>(row,nm,nc)) return true;
		if(check_2<jmc>(row,nm,nc)) return true;
		if(check_2<jmp>(row,nm,nc)) return true;
		return false;
	}
	
	int checkalg(string& filename, int& nrow, int& maxmem, int& maxcon) { //ha hibatlan az algoritmus akkor 0, ha nem, akkor az elso hibat tartalmazo sor sorszama.
		nrow=0;
		int nm=0;
		int nc=0;
		string row;
		ifstream infile;
		infile.open(filename.c_str());
		if(!infile.is_open()) return -1;
		while(!infile.eof()) {
			getline(infile,row);
			if(!checkrow(row,nm,nc)) return nrow+1;
			nrow++;
		}
		infile.close();
		maxmem=nm;
		maxcon=nc;
		return 0;
	}
}

namespace aftercheck { // uj utasitasosztalynal az addrow() fgv-t ki kell egesziteni
	template<class C>
	void addr_1(string& row, memory* mod) { // 1. OP: M[int] 2. OP: M[int]/int
		if(C::instname==nthword(row,1)) {
			int first;
			unsigned int x=6; //vizsgáló kurzor helye
			first=numberfromhere(row,x);
			x+=2;
			if(row[x]=='M') {
				x+=2;
				C* newinst=new C(mod->mem(first), mod->mem(numberfromhere(row,x)), row, first);
				mod->addinst(newinst);
			} else {
				C* newinst=new C(mod->mem(first), mod->addconst(numberfromhere(row,x)), row, first);
				mod->addinst(newinst);
			}
		}
	}

	template<class C>
	void addr_2(string& row, memory* mod) { // egyetlen OP: posint
		if(C::instname==nthword(row,1)) {
			unsigned int x=4;
			C* newinst= new C(numberfromhere(row,x), row);
			mod->addinst(newinst);
		}
	}

	void addbyrow(string& row, memory* mod) { //string alapjan adja hozza egy memory tipushoz, a string alapjan alkotott utasitast, nem vizsgalja meg a string formajat!
		addr_1<mov>(row, mod);
		addr_1<add>(row, mod);
		addr_1<cmp>(row, mod);
		addr_2<jmc>(row, mod);
		addr_2<jmp>(row, mod);
	}
}

memory* memoryfromfile(string& filename) { // fajlnevbol neumann modell (varázslat)
	int maxmem,maxcon,nrow;
	int checkresult=beforebuild::checkalg(filename, nrow, maxmem, maxcon);
	if(checkresult==0) {
		memory* neumann = new memory(nrow,maxmem,maxcon);
		ifstream infile;
		string row;
		infile.open(filename.c_str());
		for(int i=0; i<nrow; i++) {
			getline(infile,row);
			aftercheck::addbyrow(row, neumann);
		}
		infile.close();
		return neumann;
	}
	else if(checkresult==-1) throw ("file not existing");
	else if(checkresult>0) throw ("compiling error");
	return NULL;
}

}

#endif

