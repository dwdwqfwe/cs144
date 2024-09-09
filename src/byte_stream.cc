#include "byte_stream.hh"
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) :is_close(false),pre_local(0),tol_size(0),cur_size(0),capacity_( capacity ){}

bool Writer::is_closed() const
{
  return is_close;
}

void Writer::push( string data )
{
  if(available_capacity()==0||is_close||data.empty()){
    return;
  } 
  uint64_t size=data.size();
    if(size>available_capacity()){
      buf.push(data.substr(0,available_capacity()));
      tol_size+=available_capacity();
      cur_size=capacity_;
      return;
    }
    buf.push(data);
    cur_size+=size;
    tol_size+=size;
    return;
}

void Writer::close()
{
  is_close=true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_-cur_size;
}

uint64_t Writer::bytes_pushed() const
{
  return tol_size;
}

bool Reader::is_finished() const
{
  return is_close&&cur_size==0;
}

uint64_t Reader::bytes_popped() const
{
  return tol_size-cur_size;
}

string_view Reader::peek() const
{
  if(buf.empty()){
    return string_view{};
  }
  return string_view{buf.front()}.substr(pre_local);
}

void Reader::pop( uint64_t len )
{
  while(len!=0U&&!buf.empty()){
    uint64_t size=buf.front().size();
    pre_local++;
    if(pre_local>=size){
      buf.pop();
      pre_local=0;
    }
    cur_size--;
    len--;
  }
}

uint64_t Reader::bytes_buffered() const
{
  return cur_size;
}