#pragma once
#include <stdint.h>
#include <stdlib.h> 
#include <functional>
#include <utility>
#define DEFAULT_SIZE
constexpr uint32_t uint32x2Size = sizeof(uint32_t) * 2;
//  todo implement swap uudestaan

//template<typename type> 
//void xor_swap(type* lhv, type* rhv)
//{
//	if (lhv != rhv)
//	{
//		(*lhv) ^= (*rhv);
//		(*rhv) ^= (*lhv);
//		(*lhv) ^= (*rhv);
//	}
//}
//template<typename type>
//void xor_swap(type* lhv, type* rhv)
//{
//	if (lhv != rhv)
//	{
//		//(*lhv) ^= (*rhv);
//		//(*rhv) ^= (*lhv);
//		//(*lhv) ^= (*rhv);
//	}
//}

template<typename type>
inline void allocate_new_array(type** ptr, unsigned int initialSize = 20)
{
	if (initialSize == 0)initialSize = 1;
	*ptr = (type*)malloc((uint32x2Size)+sizeof(type)*initialSize);
	if (*ptr == NULL) {
		return;
	}
	((uint32_t*)(*ptr))[0] = 0;
	((uint32_t*)(*ptr))[1] = initialSize;
	*ptr = (type*)((uint32_t*)(*ptr) + 2);
}
template<typename type>
inline void push_back_dyn_array(type** ptr, const type *value)
{
	if (*((uint32_t*)(*ptr) - 2) != *((uint32_t*)(*ptr) - 1))
	{
		(*ptr)[*((uint32_t*)(*ptr) - 2)] = *value;
		*((uint32_t*)(*ptr) - 2) += 1;
	}
	else
	{
		type* temp = *ptr;
		*ptr = (type*)realloc((((uint32_t*)(*ptr)) - 2), *(((uint32_t*)(*ptr)) - 2) * 2 * sizeof(type) + uint32x2Size);
		if (*ptr == NULL) {
			free(temp);
			*ptr = NULL;
			return;
		}
		((uint32_t*)(*ptr))[1] *= 2;
		*ptr = (type*)(((uint32_t*)(*ptr)) + 2);
		(*ptr)[*((uint32_t*)(*ptr) - 2)] = *value;
		*((uint32_t*)(*ptr) - 2) += 1;
	}
}
template<typename type>
inline uint32_t get_array_size(type *ptr)
{
	return	*((uint32_t*)(ptr)-2);
}
template<typename type>
inline uint32_t get_array_allocated_size(type *ptr)
{
	return	*((uint32_t*)(ptr)-1);
}
template<typename type>
inline void free_dyn_array(type* ptr)
{
	//uint32_t* t = ((uint32_t*)(ptr) - 2);
	free(((uint32_t*)(ptr)) - 2);
}
template<typename type>
inline void popback_array(type* ptr)
{
	*((uint32_t*)(ptr)-2) -= 1;
}
template<typename type>
inline void fast_remove_element_array(type* ptr, unsigned int index)
{
	*((uint32_t*)(ptr)-2) -= 1;
	std::swap(ptr[index], ptr[*((uint32_t*)(ptr)-2)]);
	//xor_swap<type>(&ptr[index], &ptr[*((uint32_t*)(ptr)-2)]);
}
template<typename type>
inline void clear_array(type* ptr)
{
	*((uint32_t*)(ptr)-2) = 0;
}
template<typename type>
inline type* emblace_back(type** ptr)
{
	if (*((uint32_t*)(*ptr) - 2) != *((uint32_t*)(*ptr) - 1))
	{
		//*((uint32_t*)(*ptr) - 2) += 1;
		return  &((*ptr)[(*((uint32_t*)(*ptr) - 2))++]); //type* (*ptr)[*((uint32_t*)(*ptr) - 2)]
	}
	else
	{
		type* temp = *ptr;
		*ptr = (type*)realloc((((uint32_t*)(*ptr)) - 2), *(((uint32_t*)(*ptr)) - 2) * 2 * sizeof(type) + uint32x2Size);
		if (*ptr == NULL) {
			free(temp);
			*ptr = NULL;
			return NULL;
		}
		((uint32_t*)(*ptr))[1] *= 2;
		*ptr = (type*)(((uint32_t*)(*ptr)) + 2);
		return  &((*ptr)[(*((uint32_t*)(*ptr) - 2))++]); //type* (*ptr)[*((uint32_t*)(*ptr) - 2)]
	}
}

template<typename type>
inline void resize_array(type** ptr, unsigned int newSize)
{
	if (*((uint32_t*)(*ptr) - 1) > newSize)
	{
		*((uint32_t*)(*ptr) - 2) = newSize;
		return;
	}
	type* temp = *ptr;
	*ptr = (type*)realloc((((uint32_t*)(*ptr)) - 2), newSize * sizeof(type) + uint32x2Size);
	if (*ptr == NULL) {
		free(temp);
		*ptr = NULL;
		return;
	}
	((uint32_t*)(*ptr))[1] = newSize;
	*ptr = (type*)(((uint32_t*)(*ptr)) + 2);
	*((uint32_t*)(*ptr) - 2) = newSize;
}

template<typename type>
inline type* get_back(type* ptr)
{
	return ptr + *((uint32_t*)(ptr)-2) - 1;// HOW saattaa rikkoa
}
template<typename type>
inline void quick_sort_array(type* ptr, unsigned int startIndex, unsigned int size, std::function<bool(type* lhv, type* rhv)> userfunc/*bool(*userfunc)(type* lhv, type* rhv)*/)//check lambda
{
	if (size <= 2)
	{
		if (size == 2)
		{
			if (!userfunc(&ptr[startIndex], &ptr[startIndex + size - 1]))
			{
				std::swap(ptr[startIndex], ptr[startIndex + size - 1]);
				//xor_swap(&ptr[startIndex], &ptr[startIndex + size - 1]);
			}
		}
		return;
	}
	unsigned int middle = (unsigned int)(startIndex + (size / 2));
	unsigned int last = startIndex + size - 1;

	if (!userfunc(&ptr[startIndex], &ptr[middle]))
		std::swap(ptr[startIndex], ptr[middle]);
	if (!userfunc(&ptr[middle], &ptr[last]))
		std::swap(ptr[middle], ptr[last]);
	if (!userfunc(&ptr[startIndex], &ptr[middle]))
		std::swap(ptr[startIndex], ptr[middle]);


	int lower = startIndex;
	int higher = last;
	type pivot = ptr[middle];
	ptr[middle] = ptr[startIndex];
	//type* pivot = ptr + startIndex;
	while (lower < higher)
	{
		while (userfunc(&pivot, &ptr[higher]) && lower < higher)
			higher--;

		if (higher != lower)
		{
			ptr[lower] = ptr[higher];
			//std::swap(ptr[lower], ptr[higher]);
			//xor_swap<type>(&ptr[lower], &ptr[higher]);
			//pivot = &ptr[higher];
			lower++;
		}
		while (!userfunc(&pivot, &ptr[lower]) && lower < higher)
			lower++;

		if (higher != lower)
		{
			//std::swap(ptr[lower], ptr[higher]);
			ptr[higher] = ptr[lower];
			//xor_swap<type>(&ptr[lower], &ptr[higher]);
			//pivot = &ptr[lower];
			higher--;
		}
	}
	ptr[lower] = pivot;
	quick_sort_array(ptr, startIndex, lower - startIndex, userfunc);
	quick_sort_array(ptr, lower + 1, last - lower, userfunc);

}
