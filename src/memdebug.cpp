#include <memsebug.h>


static int alloc = 0;


void alloc_debug()
{
	alloc++;
}
void dealloc_debug()
{
	alloc--;
}
int get_allocations()
{
	return alloc;
}