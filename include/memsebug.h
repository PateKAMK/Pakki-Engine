#pragma once

//#define DEBUG_MEMORY_ALLOCATIONS

#ifdef DEBUG_MEMORY_ALLOCATIONS
#define malloc(v)  malloc(v); alloc_debug()
#define free(v)  free(v); dealloc_debug()


#endif // DEBUG_MEMORY_ALLOCATIONS




void alloc_debug();
void dealloc_debug();
int get_allocations();
