#include "FlatVector.h"


FlatVector::FlatVector(vector<int>& adjacency_list)//adjacency list shall contain nonnegative integers. Vertex ID shall begin with 0.
{

	//initialize fv
	fv.clear();
	//initialize fv done
	
	//build lv
	sort(adjacency_list.begin(), adjacency_list.end());//sort the vector in ascending order. This must be done before running the following code.
	if (adjacency_list.front()<0){
		cout<<"vertex ID is negative. Exit."<<endl;
		exit(-1);	
	}
	bit_track _constr_info;
	for(vector<int>::iterator it=adjacency_list.begin(); it!=adjacency_list.end(); it++)
	{
		//handling level 0
		int current_bit_level=*it;
		int current_word_level=current_bit_level/MACHINE_WIDTH;
		int current_bit_offset_level=current_bit_level%MACHINE_WIDTH;
		
		if (current_word_level>_constr_info.last_word){
			fragment new_frag;
			BitOperators::set_one(new_frag.bit_content, current_bit_offset_level);
			new_frag.start_pos=current_word_level;
			fv.push_back(new_frag);
		} else{//i.e., current_word_level==last_word_level
			if (current_bit_level>_constr_info.last_bit){
				BitOperators::set_one((fv.back()).bit_content,current_bit_offset_level);
			}
		}
		_constr_info.last_bit=current_bit_level;
		_constr_info.last_word=current_word_level;
		//handling level 0 done
	}
	//build fv done
}

FlatVector::FlatVector(const int& vertex_ID)//build using only one vertex ID.
{

	if(vertex_ID<0){
		cout<<"vertex ID is negative. Exit."<<endl;
		exit(-1);
	}
	//initialize fv
	fv.clear();
	//initialize fv done
	
	//build lv

	//handling level 0
	int current_word_level=vertex_ID/MACHINE_WIDTH;
	int current_bit_offset_level=vertex_ID%MACHINE_WIDTH;
		
	fragment new_frag;
	BitOperators::set_one(new_frag.bit_content, current_bit_offset_level);
	new_frag.start_pos=current_word_level;
	fv.push_back(new_frag);
	//handling level 0 done
	
	//build fv done
}

int FlatVector::LOGICOR(const FlatVector& target, FlatVector& result) const
{
	int number_count=0;
	int size_source=fv.size();
	int i_source=0;
	const_iterator j_target=target.begin();
	result.clear();
	while((i_source<size_source)&&(j_target!=target.end()))
	{
		fragment new_frag;
		if (fv[i_source].start_pos==j_target->start_pos){
			new_frag.bit_content=fv[i_source].bit_content| j_target->bit_content;
			new_frag.start_pos=fv[i_source].start_pos;
			number_count=number_count+BitOperators::get_number_of_ones(new_frag.bit_content);
			result.push_back(new_frag);
			i_source++;
			j_target++;
		}else if (fv[i_source].start_pos<j_target->start_pos){
			number_count=number_count+BitOperators::get_number_of_ones(fv[i_source].bit_content);
			result.push_back(fv[i_source]);
			i_source++;
		}else{//i.e., fv[i_source].start_pos>target[j_target].start_pos
			number_count=number_count+BitOperators::get_number_of_ones(j_target->bit_content);
			result.push_back(*j_target);
			j_target++;
		}
	}
	
	//At most one of the following while loop content will be accessed
	while(i_source<size_source)
	{
		number_count=number_count+BitOperators::get_number_of_ones(fv[i_source].bit_content);
		result.push_back(fv[i_source]);
		i_source++;
	}
	while(j_target!=target.end())
	{
		number_count=number_count+BitOperators::get_number_of_ones(j_target->bit_content);
		result.push_back(*j_target);
		j_target++;
	}
	//At most one of the following while loop content will be accessed done
	
	return number_count;
}

void FlatVector::LOGICAND(const FlatVector& target, FlatVector& result) const//number_count disabled in this version
{
	const_iterator i_target=this->begin();
	const_iterator j_target=target.begin();
	while((i_target!=this->end())&&(j_target!=target.end()))
	{
		fragment new_frag;
		fragment srcinfo=*i_target;
		fragment dstinfo=*j_target;
		if (srcinfo.start_pos==dstinfo.start_pos){
			new_frag.bit_content=srcinfo.bit_content & dstinfo.bit_content;
			if(new_frag.bit_content>0){
				new_frag.start_pos=srcinfo.start_pos;
				result.push_back(new_frag);
			}
			i_target++;
			j_target++;
		}else if (srcinfo.start_pos<dstinfo.start_pos){
			i_target++;
		}else{//i.e., fv[i_source].start_pos>target[j_target].start_pos
			j_target++;
		}
	}
	
	//return number_count;
}

bool FlatVector::LOGICAND(const FlatVector& target) const//number_count disabled in this version
{
	//int number_count=0;
	const_iterator i_target=this->begin();
	const_iterator j_target=target.begin();
	while((i_target!=this->end())&&(j_target!=target.end()))
	{
		fragment new_frag;
		fragment srcinfo=*i_target;
		fragment dstinfo=*j_target;
		if (srcinfo.start_pos==dstinfo.start_pos){
			new_frag.bit_content=srcinfo.bit_content & dstinfo.bit_content;
			if(new_frag.bit_content!=0)
				return true;
			i_target++;
			j_target++;
		}else if (srcinfo.start_pos<dstinfo.start_pos){
			i_target++;
		}else{//i.e., fv[i_source].start_pos>target[j_target].start_pos
			j_target++;
		}
	}
	return false;
	
	//return number_count;
}

void FlatVector::LOGICXOR(const FlatVector& target, FlatVector& result) const
{
	//int number_count=0;
	int size_source=fv.size();
	int i_source=0;
	const_iterator j_target=target.begin();
	result.clear();
	while((i_source<size_source)&&(j_target!=target.end()))
	{
		fragment new_frag;
		if (fv[i_source].start_pos==j_target->start_pos){
			new_frag.bit_content=fv[i_source].bit_content ^ j_target->bit_content;
			new_frag.start_pos=fv[i_source].start_pos;
			//number_count=number_count+BitOperators::get_number_of_ones(new_frag.bit_content);
			if(new_frag.bit_content!=0)
				result.push_back(new_frag);
			i_source++;
			j_target++;
		}else if (fv[i_source].start_pos<j_target->start_pos){
			//number_count=number_count+BitOperators::get_number_of_ones(fv[i_source].bit_content);
			result.push_back(fv[i_source]);
			i_source++;
		}else{//i.e., fv[i_source].start_pos>target[j_target].start_pos
			//number_count=number_count+BitOperators::get_number_of_ones(j_target->bit_content);
			result.push_back(*j_target);
			j_target++;
		}
	}
	
	//At most one of the following while loop content will be accessed
	while(i_source<size_source)
	{
		//number_count=number_count+BitOperators::get_number_of_ones(fv[i_source].bit_content);
		result.push_back(fv[i_source]);
		i_source++;
	}
	while(j_target!=target.end())
	{
		//number_count=number_count+BitOperators::get_number_of_ones(j_target->bit_content);
		result.push_back(*j_target);
		j_target++;
	}
	//At most one of the following while loop content will be accessed done
	
	//return number_count;
}


int FlatVector::superset(const FlatVector& target) const
{
	int size_source=fv.size();
	int i_source=0;
	const_iterator j_target=target.begin();
	bool is_superset=true;
	bool is_subset=true;

	while((i_source<size_source)&&(j_target!=target.end()))
	{
		fragment new_frag;
		if (fv[i_source].start_pos==j_target->start_pos){
			new_frag.bit_content=fv[i_source].bit_content| j_target->bit_content;
			new_frag.start_pos=fv[i_source].start_pos;
			if(new_frag.bit_content!=fv[i_source].bit_content)
				is_superset=false;
			if(new_frag.bit_content!=j_target->bit_content) 
				is_subset=false;
			
			i_source++;
			j_target++;
		}else if (fv[i_source].start_pos<j_target->start_pos){
			i_source++;
			is_subset=false;
		}else{//i.e., fv[i_source].start_pos>target[j_target].start_pos
			j_target++;
			is_superset=false;
		}
	}
	
	//At most one of the following while loop content will be accessed
	while(i_source<size_source)
	{
		i_source++;
		is_subset=false;
	}
	while(j_target!=target.end())
	{
		j_target++;
		is_superset=false;
	}
	//At most one of the following while loop content will be accessed done
	
	if((is_superset==true)&&(is_subset==false))
		return 1;//superset
	else if ((is_superset==false)&&(is_subset==true))
		return -1;//subset
	else if ((is_superset==true)&&(is_subset==true))
		return 0;//equal
	else{ //means ((is_supset==true)&&(is_subset==true));
		return -2;//neither superset nor subset
	}
}

bool FlatVector::isLess(const FlatVector& target) const
{
	{//true means this<target; false means this>=target
		FlatVector::const_reverse_iterator itl=fv.rbegin();
		FlatVector::const_reverse_iterator itr=target.rbegin();
		while(itl!=fv.rend() && itr!=target.rend()){
			if (itl->start_pos<itr->start_pos)
				return true;
			else if (itl->start_pos>itr->start_pos)
				return false;
			else{//itl->start_pos==itr->start_pos
				if (itl->bit_content<itr->bit_content)
					return true;
				else if (itl->bit_content>itr->bit_content)
					return false;
				else{//itl->bit_content==target->bit_content
					itl++;
					itr++;
				}
			}
		}
		if (itr!=target.rend())
			return true;
		else
			return false;
	}
}

void FlatVector::insert(const int& vertex_ID)
{
	if(vertex_ID<0){
		cout<<"vertex ID is negative. Exit."<<endl;
		exit(-1);
	}
	
	//build lv

	//handling level 0
	int current_word_level=vertex_ID/MACHINE_WIDTH;
	int current_bit_offset_level=vertex_ID%MACHINE_WIDTH;
		
	for(vector<fragment>::iterator it=fv.begin(); it!=fv.end(); it++){
		if (it->start_pos==current_word_level){
			BitOperators::set_one(it->bit_content,current_bit_offset_level);
			return;
		}
	}
		
	fragment new_frag;
	BitOperators::set_one(new_frag.bit_content, current_bit_offset_level);
	new_frag.start_pos=current_word_level;
	
	for(vector<fragment>::iterator it=fv.begin(); it!=fv.end(); it++){
		if(new_frag.start_pos<it->start_pos){
			fv.insert(it,new_frag);
			return;
		}
	}
	//if not return, it implies new_frag shall be placed at the end.
	
	fv.push_back(new_frag);
	//handling level 0 done
	
	//build fv done
	
}

void FlatVector::set_one(const int& bit_position){
	if(bit_position<0){
		cout<<"FlatVector set_one receives a negative position. Exit."<<endl;
		exit(-1);
	}
	int bit_position_word_level=bit_position/(MACHINE_WIDTH/8);
	int bit_position_offset_level=bit_position%(MACHINE_WIDTH/8);
	
	for(vector<fragment>::iterator it=fv.begin(); it!=fv.end(); it++){
		if (it->start_pos==bit_position_word_level){
			BitOperators::set_one(it->bit_content,bit_position_offset_level);
			return;
		}
	}
	
	fragment new_frag;
	BitOperators::set_one(new_frag.bit_content, bit_position_offset_level);
	new_frag.start_pos=bit_position_word_level;
	
	for(vector<fragment>::iterator it=fv.begin(); it!=fv.end(); it++){
		if(new_frag.start_pos<it->start_pos){
			fv.insert(it,new_frag);
			return;
		}
	}
	
	fv.push_back(new_frag);
}

void FlatVector::set_zero(const int& bit_position){
	if(bit_position<0){
		cout<<"FlatVector set_zero receives a negative position. Exit."<<endl;
		exit(-1);
	}
	int bit_position_word_level=bit_position/(MACHINE_WIDTH/8);
	int bit_position_offset_level=bit_position%(MACHINE_WIDTH/8);
	
	for(vector<fragment>::iterator it=fv.begin(); it!=fv.end(); it++){
		if (it->start_pos==bit_position_word_level){
			BitOperators::set_zero(it->bit_content,bit_position_offset_level);
			return;
		}
	}
	
}

void FlatVector::set_zero_one(const int& bit_position_zero, const int& bit_position_one){
	if(bit_position_zero<0 || bit_position_one<0){
		cout<<"FlatVector set_zero_one receives a negative position. Exit."<<endl;
		exit(-1);
	}
	int bit_position_zero_word_level=bit_position_zero/(MACHINE_WIDTH/8);
	int bit_position_one_word_level=bit_position_one/(MACHINE_WIDTH/8);
	if(bit_position_zero_word_level!=bit_position_one_word_level){
		set_zero(bit_position_zero);
		set_one(bit_position_one);
		return;
	}
	
	//bit_position_zero_word_level == bit_position_one_word_level for the following
	int bit_position_zero_offset_level=bit_position_zero%(MACHINE_WIDTH/8);
	int bit_position_one_offset_level=bit_position_one%(MACHINE_WIDTH/8);

	for(vector<fragment>::iterator it=fv.begin(); it!=fv.end(); it++){
		if (it->start_pos==bit_position_zero_word_level){
		BitOperators::set_zero(it->bit_content,bit_position_zero_offset_level);
		BitOperators::set_one(it->bit_content,bit_position_one_offset_level);
		return;
		}
	}
	
	fragment new_frag;
	BitOperators::set_one(new_frag.bit_content, bit_position_one_offset_level);
	new_frag.start_pos=bit_position_one_word_level;
	
	for(vector<fragment>::iterator it=fv.begin(); it!=fv.end(); it++){
		if(new_frag.start_pos<it->start_pos){
			fv.insert(it,new_frag);
			return;
		}
	}
	
	fv.push_back(new_frag);
}
