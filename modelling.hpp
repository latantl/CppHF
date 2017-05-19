#ifndef MODELLING_HPP
#define MODELLING_HPP

#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "rlutil.h"
#include "neumann.hpp"

using namespace std;
using namespace rlutil;
using namespace neumann;

int pressed_ent_esc() {
	int key=-1;
	while(key!=KEY_ENTER && key!=KEY_ESCAPE) key=getkey();
	return key;
}
void readkeyuntil(int i) {
	int c=-1;
	while(c!=i) c=getkey();
}
int instmaxlength(memory* mod) {
	unsigned int x=0;
	for(int i=1; i<=mod->getNINST(); i++) {
		string str=mod->inst(i)->getRow();
		if(str.length()>x) x=str.length();
	}
	return x;
}
void startlist(memory* mod, int instmax) {
	cls();
	setBackgroundColor(BLACK);
	setColor(WHITE);
	for(int i=1; i<=mod->getNINST(); i++) {
		gotoxy(i+1,2);
		cout << mod->inst(i)->getRow();
	}
	for(int i=1; i<=mod->getSMEM(); i++) {
		gotoxy(i+1,3+instmax);
		cout << setw(2) << i << ": " << *(mod->mem(i));
	}
	gotoxy(2,instmax+14);
	cout << "cntr: 0";
	gotoxy(3,instmax+14);
	cout << "cond: false";
}
void list(memory* mod, int instmax, int pos, bool ih, int chmem) {
	setBackgroundColor(BLACK);
	cls();
	setColor(WHITE);
	for(int i=1; i<=mod->getNINST(); i++) {
		gotoxy(i+1,2);
		if(i==pos) setBackgroundColor(BLUE);
		else setBackgroundColor(BLACK);
		cout << mod->inst(i)->getRow();
	}
	for(int i=1; i<=mod->getSMEM(); i++) {
		gotoxy(i+1,3+instmax);
		if(i==chmem) setBackgroundColor(GREEN);
		else setBackgroundColor(BLACK);
		cout << setw(2) << i << ": " << *(mod->mem(i));
	}
	gotoxy(2,instmax+14);
	setBackgroundColor(GREEN);
	cout << "cntr: " << pos;
	gotoxy(3,instmax+14);
	if(ih) setBackgroundColor(GREEN);
	else setBackgroundColor(BLACK);
	cout << "cond: ";
	if(mod->tokenbool()) cout << "true";
	else cout << "false";
}
void filelist(memory* mod, int n, int pos, bool ih, int chmem, ostream& file) {
	file << n << ": " << pos << "; " << ih << "; " << mod->inst(pos)->getRow();
	for(int i=1; i<=mod->getSMEM(); i++) {
		file << "; ";
		if(chmem==i) file << "*";
		file << "MEM[" << i << "]=" << *(mod->mem(i));
	}
	file << endl;
}
void endscreen(memory* neumann) {
	gotoxy(3+max(neumann->getNINST(),neumann->getSMEM()),1);
	setColor(WHITE);
	setBackgroundColor(RED);
	cout << "ENDED";
	anykey();
	setBackgroundColor(BLACK);
	cls();
}
void modelling(string filename, memory* neumann) {
	ofstream outfile;
	filename+="_result.txt";
	outfile.open(filename.c_str());
	int instmax=instmaxlength(neumann);
	startlist(neumann, instmax);
	int pos=neumann->tokenpos();
	bool cond=neumann->tokenbool();
	int chmem=neumann->inst(pos)->changedmem();
	if(pressed_ent_esc()==KEY_ESCAPE) return;
	int n=1;
	while(neumann->next()) {
		list(neumann, instmax, pos, cond!=neumann->tokenbool(), chmem);
		filelist(neumann, n , pos, cond, chmem, outfile);
		n++;
		pos=neumann->tokenpos();
		cond=neumann->tokenbool();
		if(pos>=1 && pos<=neumann->getNINST())
			chmem=neumann->inst(pos)->changedmem();
		if(pressed_ent_esc()==KEY_ESCAPE) {
			outfile << "aborted";
			outfile.close();
			return;
		}
	}
	outfile.close();
	endscreen(neumann);
}
void stmodelling(memory* neumann) {
	int pos=neumann->tokenpos();
	bool cond=neumann->tokenbool();
	int chmem=neumann->inst(pos)->changedmem();
	int n=1;
	while(neumann->next()) {
		filelist(neumann, n , pos, cond, chmem, std::cout);
		n++;
		pos=neumann->tokenpos();
		cond=neumann->tokenbool();
		if(pos>=1 && pos<=neumann->getNINST()) chmem=neumann->inst(pos)->changedmem();
	}
}

#endif
