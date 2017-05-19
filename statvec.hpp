#ifndef STATVEC_HPP
#define STATVEC_HPP

#include <iostream>

template<class C>
class statvec {
	int siz;
	int n;
	C* Data;
public:
	statvec(unsigned int op=0) :siz(op), n(0) { Data= new C[siz]; }
	statvec(statvec& masik) {
		siz=masik.siz;
		n=masik.n;
		for(int i=0; i<n; i++) Data[i]=masik.Data[i];
	}
	~statvec() { delete[] Data; }
	statvec& operator=(const statvec& masik) {
		delete[] Data;
		siz=masik.siz;
		n=masik.n;
		for(int i=0; i<n; i++) Data[i]=masik.Data[i];
	}
	statvec& operator+=(C& uj) {
		if(n<siz) {
			Data[n++]=uj;
		} else throw ("tomb: sikertelem elem hozzaadas, a tomb tele van");
	}
	C& operator[](int i) const {
		if(i<0 || i>siz-1) throw ("Tomb: tulindexeles");
		return Data[i];
	}
	C& operator[](int i) {
		if(i<0 || i>siz-1) throw ("Tomb: tulindexeles");
		return Data[i];
	}
	C& last() const { return Data[n-1]; }
	C& last() { return Data[n-1]; }
	int getN() const { return n; }
	int getS() const { return siz; }
	bool empty() const { return n==0; }
	bool full() const { return n==siz; }
	void add(const C& uj) { if(!this->full()) Data[n++]=uj; }
	void editsize(int op) {
		if(op<siz) {
			C* newData= new C[op];
			for(int i=0; i<n && i<op; i++) newData[i]=Data[i];
			delete[] Data;
			Data=newData;
			siz=op;
		}
		if(op>siz) {
			C* newData= new C[op];
			for(int i=0; i<n; i++) newData[i]=Data[i];
			delete[] Data;
			Data=newData;
			siz=op;
		}
	}
};

#endif
