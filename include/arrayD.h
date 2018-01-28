#pragma once
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

template<typename T>
class dynamicArray final
{
public:
	T*			data;
private:
	uint32_t	_allocated_size;
	uint32_t	_size;

public:
	dynamicArray(uint32_t init_size = 10) : _size(0), _allocated_size(0), data(nullptr){};
	~dynamicArray()
	{
		if(data) free(data);
	}

	void init_array(uint32_t init_size = 10)
	{
		_size = 0;
		data = (T*)malloc(sizeof(T) * init_size);
		_allocated_size = init_size;
	}
	void dispose_array()
	{
		free(data);
		data = NULL;
	}
	uint32_t get_size()
	{
		return _size;
	}

	uint32_t get_allocated_size()
	{
		return _allocated_size;
	}

	void push_back(T& obj)
	{
		if (_allocated_size == _size)
		{
			_allocated_size *= 2;
			T* temp = data;
			data = (T*)realloc(data, sizeof(T) *(size_t)_allocated_size);
			assert(data);
			if(!data)
			{
				data = temp;
			}
		}
		data[_size++] = obj;
	}

	void delete_back()
	{
		assert(_size > 0);
		_size--;
	}
	T* get_back()
	{
		if (_size > 0)
		{
			return &data[_size - 1];
		}
		return nullptr;
	}
	void clear_array()
	{
		_size = 0;
	}
	void resize_array(uint32_t new_size)
	{
		if (new_size > _allocated_size)
		{
			_allocated_size = new_size;
			realloc(data, _sizeof(T) *(size_t)_allocated_size);
		}
	}
	T* get_new_item()
	{
		if (_allocated_size == _size)
		{
			_allocated_size *= 2;
			T* temp = data;
			data = (T*)realloc(data, sizeof(T) *(size_t)_allocated_size);
			assert(data);
			if (!data)
			{
				data = temp;
			}
		}
		return &data[_size++];
	}
	void fast_remove(uint32_t index)
	{
		assert(index < _size);
		if(index == _size - 1) 
		{
			_size--;
		}
		else
		{
			data[index] = data[_size--];
		}

	}
};

//todo
//[] operaattori?
//copy construcrori?
//itaraattori?
//quicksort?
//resize vähentää kokoa?
//pidetäänkö push back reffinä?

// test get_new item


template<typename T, const uint32_t poolSize = 50>
class pool final
{
public:

private:
	uint32_t			_currentArraySize;
	dynamicArray<T*>	_data;
	dynamicArray<T*>	_freelist;
public:
	pool() :_currentArraySize(0) {}
	~pool()
	{
        if(_data.data)
        {      
            for (uint32_t i = 0; i < _data.get_size(); i++)
            {
                if (_data.data[i])free(_data.data[i]);
            }
        }
	}
	void init_pool()
	{
		_currentArraySize = 0;
		T* newdata = (T*)malloc(sizeof(T)* poolSize);
        _data.init_array();
        _freelist.init_array();
		_data.push_back(newdata);
	}
	void dispose_pool()
	{
		for (uint32_t i = 0; i < _data.get_size(); i++)
		{
			free(_data.data[i]);
		}
		_data.dispose_array();
		_freelist.dispose_array();
	}
	T* new_item()
	{
		if (_freelist.get_size())
		{
			T** freeObj = _freelist.get_back();
			_freelist.delete_back();
			return *freeObj;
		}

		if (_currentArraySize >= poolSize)
		{
			_currentArraySize = 0;
			T* newdata = (T*)malloc(sizeof(T)* poolSize);
			_data.push_back(newdata);
		}
		T** currentArray = _data.get_back();
		return &((*currentArray)[_currentArraySize++]);
	}

	void delete_obj(T *obj)
	{
		_freelist.push_back(obj);
	}

	uint32_t get_pool_size()
	{
		return (_data.get_size() -1)* poolSize + _currentArraySize;
	}

	uint32_t get_allocated_size()
	{
		return _data.get_size()* poolSize;
	}
};
//
//template<typename T>
//struct dynArr
//{
//	T*			data;
//	uint32_t	size;
//	uint32_t	allocatedsize;
//};
//template<typename T>
//void initDyArr(dynArr<T>* arr,uint32_t initsize = 10)
//{
//	arr->data = NULL;
//	arr->size = 0;
//	arr->allocatedsize = 0;
//	arr->data = (T*)malloc(sizeof(T) * initsize);
//	arr->allocatedsize = initsize;
//}
//
//template<typename T>
//T* get_next_item(dynArr<T>* arr)
//{
//	if (arr->allocatedsize == arr->size)
//	{
//		arr->allocatedsize *= 2;
//		T* temp = arr->data;
//		arr->data = (T*)realloc(arr->data, sizeof(T) *arr->allocatedsize);
//		assert(data);
//		if (!data)
//		{
//			arr->data = temp;
//		}
//	}
//	return arr->data[arr->size++];
//}
//
//template<typename T>
//void dispose_array(dynArr<T>* arr)
//{
//	free(arr->data);
//	memset(arr->data, 0, sizeof *arr);
//}
//template<typename T>
//void free_array(dynArr<T>* arr)
//{
//	arr->size = 0;
//}
//
//template<typename T>
//struct memAllocator
//{
//	uint32_t			currentArraySize;
//	dynamicArray<T*>	data;
//	dynamicArray<T*>	freelist;
//};
//
//template<typename T>
//void init_memallocator(memAllocator<T>* p,uint32_t poolsize)
//{
//	initDyArr<T*>(p->data, 5);
//	initDyArr<T*>(p->freelist, poolsize);
//	T* d = (T*)malloc(sizeof(T) * poolsize);
//	T* current = get_next_item<T*>(p->data);
//	current = d;
//}
//
//template<>

//todo
//checkkaa onko deletöity pointer valid?
//get size palauttaa oikean koon vai max koon?

