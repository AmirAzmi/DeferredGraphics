#pragma once
#include <type_traits>
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
  template <typename T, typename ...S>
  T * TAllocate(S&&...args);

  template <typename T>
  void TDeallocate(T*);

  LinearAllocator& operator=(LinearAllocator&&) = delete;
  LinearAllocator(LinearAllocator&&) = delete;
  LinearAllocator& operator=(const LinearAllocator&) = delete;
  LinearAllocator(const LinearAllocator&) = delete;
};


//give out the pointer to the main memory start
//allocate just moves that pointer up within the main memory
//clear resets the pointer back to the start of the given out main memory pointer

class LinearAllocatorScope
{
  LinearAllocator& allocator;
  char* current_memory_offset;
public:

  LinearAllocatorScope(LinearAllocator& allocator);
  ~LinearAllocatorScope();

 LinearAllocatorScope& operator=(LinearAllocatorScope&&) = delete;
 LinearAllocatorScope(LinearAllocatorScope&&) = delete;
 LinearAllocatorScope& operator=(const LinearAllocatorScope&) = delete;
 LinearAllocatorScope(const LinearAllocatorScope&) = delete;

};

template<typename T, typename ...S>
inline T * LinearAllocator::TAllocate(S&& ...args)
{
  //placement new for the pointer type of allocate and passing the constructor args of that type
  return new (Allocate(sizeof(T))) T(std::forward<S>(args)...);
  //return new T(std::forward<S>(args)...);
}

template<typename T>
inline void LinearAllocator::TDeallocate(T* arg)
{
  if (arg != nullptr)
  {
    arg->~T();
  }
  //delete arg;
}
