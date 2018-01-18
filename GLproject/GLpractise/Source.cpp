#include "Header.h"
#include <stdio.h>

int main()
{
	dynamicArray<int> arr;
	for(int i = 0;i < 100; i++)
	{
		arr.push_back(i);
	}
	for (int i = 0; i < 100; i++)
	{
		printf("num : %d\n", arr.data[i]);
	}
	printf("back : %d\n", *arr.get_back());
	printf("size : %d\n", arr.get_size());
	printf("allocated size : %d\n", arr.get_allocated_size());
	pool<int,10> intPool;
	for(int i = 0;i < 15;i++)
	{
		intPool.insert(arr.data[i]);
	}
	printf("size : %d\n", intPool.get_pool_size());
	printf("allocated size : %d\n", intPool.get_allocated_size());

	getchar();
	return 0;
}