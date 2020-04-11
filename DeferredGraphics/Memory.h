#pragma once
#include "LinearAllocator.h"

//true global
//extern means its defined in some cpp file, go look for it
//the extern make sures there is only one allocator of that type at a time
extern LinearAllocator linearAllocator;