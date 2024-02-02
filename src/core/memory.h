#pragma once

#include <banjo/memory.h>

#define bjNewStruct(type, allocator) bjAllocate(sizeof(struct type##_T), allocator)
#define bjNew(type, allocator) bjAllocate(sizeof(type), allocator)
#define bjNewN(type, count, allocator) bjAllocate(sizeof(type) * count, allocator)
