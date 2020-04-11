#include "LinearAllocator.h"
#include <cassert>
LinearAllocator::LinearAllocator(const int size) :memory_size(size)
{
  main_memory = new char[size];
  current_main_memory = main_memory;
}

char* LinearAllocator::Allocate(int size)
{
  //cant allocate more than the total given size
  if (size > memory_size)
  {
    assert(!"The amount you were trying to allocate is greater than the main memory size.");
  }

  //temp pointer given out which is temp_main_mem + size of allocated chunk
  char* given_out_memory = current_main_memory;

  //current size is less than the main memory size
  if (current_main_memory - main_memory <= memory_size)
  {
    current_main_memory += size; //current location of pointer incremented
    return given_out_memory;
  }

  //cant allocate more than the total given size after multiple allocations
  assert(!"Current Memory Size was greater than Main Memory Size after the total Allocations.");

}

void LinearAllocator::Clear()
{
  //moves pointer for temp memory all the way back to the start of main memory
  current_main_memory = main_memory;
}

LinearAllocator::~LinearAllocator()
{
  delete [] main_memory;
}

LinearAllocatorScope::LinearAllocatorScope(LinearAllocator & allocator): allocator(allocator)
{
  //set the offset at the current spot it is in
  current_memory_offset = allocator.current_main_memory;
}

LinearAllocatorScope::~LinearAllocatorScope()
{
  //move the current main memeory pointer to the old current offset of where it was at
  allocator.current_main_memory = current_memory_offset;
}
