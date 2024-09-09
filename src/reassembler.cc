#include "reassembler.hh"
#include <cstdint>
#include <utility>
#include<iostream>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if(data.size()+bytes_pended>output_.writer().available_capacity()||first_index+data.size()>=first_unaccept+1
  ||first_index+data.size()<commit_index+1){
    return;
  }

  if(is_last_substring){
    init_flag=true;
    max_index=first_index+data.size()-1;
  }
  uint64_t begin=first_index>=commit_index?first_index:commit_index;
  data=data.substr(begin-first_index);
  for(uint64_t i=0;i<data.size();i++){
    store_map[first_index+i]=data[i];
  }
  first_unpushed=first_unpushed<begin?first_unpushed:begin;
  first_unaccept=first_unpushed+output_.writer().available_capacity();
  bytes_pended+=data.size();
  send_next();

}

uint64_t Reassembler::bytes_pending() const
{
  return bytes_pended;
}
void Reassembler::send_next()
{
  string temp;
  while(store_map.find(commit_index)!=store_map.end()&&commit_index<first_unaccept){
    temp+=store_map[commit_index];;
    bytes_pended--;
    commit_index++;
    if(commit_index>max_index)
      break;
  }
  if(!temp.empty())
    output_.writer().push(temp);
  first_unpushed+=temp.size();
  first_unaccept=first_unpushed+output_.writer().available_capacity();
  if(commit_index>max_index){
      output_.writer().close();
    }
}


