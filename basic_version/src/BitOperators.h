#ifndef _BIT_OPERATORS_H_
#define _BIT_OPERATORS_H_
#include <sys/time.h>
#include <iostream>
#include <vector>

#define MACHINE_WIDTH 64

using namespace std;
typedef uint64_t MACHINE_WORD;


class BitOperators{
	private:
		static vector<vector<uint16_t> > fast_16bits_lookup_table_pos;// a number --> the positions of '1'
		static vector<vector<uint16_t> > fast_16bits_lookup_table_rank;// a number --> the ranks of '1'
		static void intialize_16bits_lookup_tables();
	public:
		static void set_one(MACHINE_WORD&, const int&);
		static void set_zero(MACHINE_WORD&, const int&);
		static bool bitRead(const MACHINE_WORD&, const int&);
		static void get_ones(MACHINE_WORD, vector<int>&);//Use a copy of MACHINE_WORD to perform shift operation
		static void get_ones_startpos(MACHINE_WORD, vector<int>&, const int&);//Use a copy of MACHINE_WORD to perform shift operation
		static int get_number_of_ones(MACHINE_WORD); //Use a copy of MACHINE_WORD to perform shift operation
		static void get_offsets_AND(const MACHINE_WORD&, const MACHINE_WORD&, vector<int>&, vector<int>&);
		static void get_offsets_AND_append(const MACHINE_WORD&, const MACHINE_WORD&, vector<int>&, vector<int>&, const int&, const int&);
};

inline
void BitOperators::set_one(MACHINE_WORD& source, const int& bit_ind)//index is from left to right and starts from 0
{
	uint64_t _bit_pointer=1;
	source |= (_bit_pointer<< bit_ind);
}

inline
void BitOperators::set_zero(MACHINE_WORD& source, const int& bit_ind)//index is from left to right and starts from 0
{
	uint64_t _bit_pointer=1;
	source &= ~(_bit_pointer<< bit_ind);
}

inline
bool BitOperators::bitRead(const MACHINE_WORD& source, const int& bit_ind)//index is from left to right and starts from 0
{
	uint64_t _bit_pointer=1;
	MACHINE_WORD tmp= source &(_bit_pointer<< bit_ind);
	if(tmp==0)
		return false;
	else
		return true;
}
#endif
