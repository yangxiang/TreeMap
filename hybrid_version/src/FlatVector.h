#ifndef _FLAT_VECTOR
#define _FLAT_VECTOR

#include <stdint.h>
#include <iostream>
#include "BitOperators.h"
#include <vector>
#include <algorithm>

#define MAXIMUM_NUMBER_TO_HANDLE 10000000

struct fragment {
	fragment():bit_content(0), start_pos(0){};
	MACHINE_WORD bit_content;
	int	start_pos;
};

struct bit_track{
	bit_track():last_bit(-1), last_word(-1){};
	int last_bit;
	int last_word;
};

class FlatVector
{

protected:
	vector<fragment> fv;

public:
	FlatVector();
	FlatVector(vector<int>&);
	FlatVector(const int&);
	~FlatVector();
	typedef vector<fragment>::const_iterator const_iterator;
	typedef vector<fragment>::const_reverse_iterator const_reverse_iterator;
	const_iterator begin() const;
	const_iterator end() const;
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;
	size_t size() const;//size of fv
	size_t block_size();
	fragment& operator[](const int);
	const fragment& operator[](const int) const;
	FlatVector& operator=(const FlatVector&);
	void clear();
	void resize(size_t);
	void push_back(const fragment& new_frag);
	int LOGICOR(const FlatVector&, FlatVector&) const;//first: target; second: result
	void LOGICAND(const FlatVector&, FlatVector&) const;//first: target; second: result
	bool LOGICAND(const FlatVector&) const;//first: target; If not zero, return true; otherwise false.
	void LOGICXOR(const FlatVector&, FlatVector&) const;//first: target; second: result. NOTE THIS FUNCTION HAVE NOT BEEN TESTED
	void getNumbers(vector<int>&) const;
	bool isLess(const FlatVector&) const;
	void insert(const int&);
	int superset(const FlatVector&) const;
	void set_one(const int&);
	void set_zero(const int&);
	void set_zero_one(const int&, const int&);
};

inline
FlatVector::FlatVector(){
}

inline
FlatVector::~FlatVector(){
	fv.clear();
}

inline
FlatVector::const_iterator FlatVector::begin() const{
	return fv.begin();
}

inline
FlatVector::const_iterator FlatVector::end() const{
	return fv.end();
}

inline
FlatVector::const_reverse_iterator FlatVector::rbegin() const{
	return fv.rbegin();
}

inline
FlatVector::const_reverse_iterator FlatVector::rend() const{
	return fv.rend();
}

inline
size_t FlatVector::size() const{
	return fv.size();
}

inline
size_t FlatVector::block_size(){
	return fv.size();
}

inline
fragment& FlatVector::operator[](const int index) {
	return fv[index];
}

inline
const fragment& FlatVector::operator[](const int index) const{
	return fv[index];
}

inline
void FlatVector::clear(){
	fv.clear();
}

inline
void FlatVector::resize(size_t new_size){
	fv.resize(new_size, fragment());
}

inline
void FlatVector::push_back(const fragment& new_frag){
	fv.push_back(new_frag);
}

inline
FlatVector& FlatVector::operator=(const FlatVector& f) {
	if (this != &f) {
		fv = f.fv;
	}
	return *this;
}

inline
void FlatVector::getNumbers(vector<int>& Numbers) const
{
	Numbers.clear();
	for(vector<fragment>::const_iterator it=fv.begin(); it!=fv.end(); it++){
		BitOperators::get_ones_startpos(it->bit_content, Numbers, it->start_pos);
	}
}
#endif
