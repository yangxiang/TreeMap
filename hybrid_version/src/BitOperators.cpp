#include "BitOperators.h"

vector<vector<uint16_t> > BitOperators::fast_16bits_lookup_table_pos;//static member variable must be declared here
vector<vector<uint16_t> > BitOperators::fast_16bits_lookup_table_rank;

void BitOperators::intialize_16bits_lookup_tables()
{
	
	if((fast_16bits_lookup_table_pos.size()>0)&&(fast_16bits_lookup_table_rank.size()>0)){
		cout<<"fast lookup tables have already been intialized. Return."<<endl;
		return;
	}
	cout<<"Initializing 16 bits lookup tables."<<endl;
	fast_16bits_lookup_table_pos.clear();
	fast_16bits_lookup_table_pos.resize(65536,vector<uint16_t>());
	fast_16bits_lookup_table_rank.clear();
	fast_16bits_lookup_table_rank.resize(65536,vector<uint16_t>(16,-1));
	
	for(uint64_t entry=0; entry<=65535; entry++)
	{
		int rank=0;
		for(uint16_t j=0; j<=15; j++)
		{
			if(bitRead(entry,j)){
				fast_16bits_lookup_table_pos[entry].push_back(j);
				fast_16bits_lookup_table_rank[entry][j]=rank;
				rank++;
			}
		}
	}
}

void BitOperators::get_ones(MACHINE_WORD source, vector<int>& positions)
{
	if((fast_16bits_lookup_table_pos.size()==0)||(fast_16bits_lookup_table_rank.size()==0))
		intialize_16bits_lookup_tables();
	
	int units=sizeof(MACHINE_WORD)/2;
	units=units+sizeof(MACHINE_WORD)%2;
	positions.clear();
	for (int i=0; i<units; i++){
		uint16_t tmp=(uint16_t)source;
		for (vector<uint16_t>::iterator it=fast_16bits_lookup_table_pos[tmp].begin(); it!=fast_16bits_lookup_table_pos[tmp].end(); it++){
			positions.push_back(*it+16*i);
		}
		source=source>>16;
	}

}

void BitOperators::get_ones_startpos(MACHINE_WORD source, vector<int>& positions, const int& start_pos)
{
	if((fast_16bits_lookup_table_pos.size()==0)||(fast_16bits_lookup_table_rank.size()==0))
		intialize_16bits_lookup_tables();
	
	int units=sizeof(MACHINE_WORD)/2;
	units=units+sizeof(MACHINE_WORD)%2;
	for (int i=0; i<units; i++){
		uint16_t tmp=(uint16_t)source;
		for (vector<uint16_t>::iterator it=fast_16bits_lookup_table_pos[tmp].begin(); it!=fast_16bits_lookup_table_pos[tmp].end(); it++){
			positions.push_back(*it+16*i+start_pos*MACHINE_WIDTH);
		}
		source=source>>16;
	}

}

int BitOperators::get_number_of_ones(MACHINE_WORD source)
{
	if((fast_16bits_lookup_table_pos.size()==0)||(fast_16bits_lookup_table_rank.size()==0))
		intialize_16bits_lookup_tables();
	
	int units=sizeof(MACHINE_WORD)/2;
	units=units+sizeof(MACHINE_WORD)%2;
	int number_of_ones=0;
	for (int i=0; i<units; i++){
		uint16_t tmp=(uint16_t)source;
		number_of_ones+=fast_16bits_lookup_table_pos[tmp].size();
		source=source>>16;
	}
	return number_of_ones;
}

void BitOperators::get_offsets_AND(const MACHINE_WORD& a, const MACHINE_WORD& b, vector<int>& offset_a, vector<int>& offset_b)
{
	if((fast_16bits_lookup_table_pos.size()==0)||(fast_16bits_lookup_table_rank.size()==0))
		intialize_16bits_lookup_tables();
		
	offset_a.clear();
	offset_b.clear();
	int units=sizeof(MACHINE_WORD)/2;
	units=units+sizeof(MACHINE_WORD)%2;
	
	uint16_t a_tmp[units], b_tmp[units];//divide the machine word into several sections
	int a_start_pos[units], b_start_pos[units];//record how many 1's before this section
	int number_of_ones_a=0;
	int number_of_ones_b=0;
	for (int i=0; i<units; i++){
		a_start_pos[i]=number_of_ones_a;
		b_start_pos[i]=number_of_ones_b;
		a_tmp[i]=(uint16_t)(a>>(16*i));
		b_tmp[i]=(uint16_t)(b>>(16*i));
		number_of_ones_a+=fast_16bits_lookup_table_pos[a_tmp[i]].size();
		number_of_ones_b+=fast_16bits_lookup_table_pos[b_tmp[i]].size();
	}

	
	MACHINE_WORD c=a & b;
	vector<uint16_t> positions;
	for (int i=0; i<units; i++){
		uint16_t tmp=(uint16_t)c;
		for (vector<uint16_t>::iterator it=fast_16bits_lookup_table_pos[tmp].begin(); it!=fast_16bits_lookup_table_pos[tmp].end(); it++){
			offset_a.push_back(fast_16bits_lookup_table_rank[a_tmp[i]][*it]+a_start_pos[i]);
			offset_b.push_back(fast_16bits_lookup_table_rank[b_tmp[i]][*it]+b_start_pos[i]);
		}
		c=c>>16;
	}
}

void BitOperators::get_offsets_AND_append(const MACHINE_WORD& a, const MACHINE_WORD& b, vector<int>& offset_a, vector<int>& offset_b, const int& start_pos_a, const int& start_pos_b)
{
	if((fast_16bits_lookup_table_pos.size()==0)||(fast_16bits_lookup_table_rank.size()==0))
		intialize_16bits_lookup_tables();

	//No vector clear up in this function
	//offset_a.clear();
	//offset_b.clear();
	
	int units=sizeof(MACHINE_WORD)/2;
	units=units+sizeof(MACHINE_WORD)%2;
	
	uint16_t a_tmp[units], b_tmp[units];//divide the machine word into several sections
	int a_start_pos[units], b_start_pos[units];//record how many 1's before this section
	int number_of_ones_a=0;
	int number_of_ones_b=0;
	for (int i=0; i<units; i++){
		a_start_pos[i]=number_of_ones_a;
		b_start_pos[i]=number_of_ones_b;
		a_tmp[i]=(uint16_t)(a>>(16*i));
		b_tmp[i]=(uint16_t)(b>>(16*i));
		number_of_ones_a+=fast_16bits_lookup_table_pos[a_tmp[i]].size();
		number_of_ones_b+=fast_16bits_lookup_table_pos[b_tmp[i]].size();
	}

	
	MACHINE_WORD c=a & b;
	vector<uint16_t> positions;
	for (int i=0; i<units; i++){
		uint16_t tmp=(uint16_t)c;
		for (vector<uint16_t>::iterator it=fast_16bits_lookup_table_pos[tmp].begin(); it!=fast_16bits_lookup_table_pos[tmp].end(); it++){
			offset_a.push_back(fast_16bits_lookup_table_rank[a_tmp[i]][*it]+a_start_pos[i]+start_pos_a);
			offset_b.push_back(fast_16bits_lookup_table_rank[b_tmp[i]][*it]+b_start_pos[i]+start_pos_b);
		}
		c=c>>16;
	}
}
