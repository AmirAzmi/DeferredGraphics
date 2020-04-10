#pragma once
class LinearAllocator
{
public:
  char* main_memory;
  char* current_main_memory;
  int memory_size;

  LinearAllocator(const int size);
  char* Allocate(int size);
  void Clear(); //reset the offset to 0
  ~LinearAllocator();
};


//give out the pointer to the main memory start
//allocate just moves that pointer up within the main memory
//clear resets the pointer back to the start of the given out main memory pointer

class LinearAllocatorScope
{
  LinearAllocator& allocator;
  char* current_memory_offset;

  LinearAllocatorScope(LinearAllocator& allocator);
  ~LinearAllocatorScope();

};
