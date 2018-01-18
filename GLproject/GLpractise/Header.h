#pragma once
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

template<typename T>
class dynamicArray
{
public:
	T*			data;
private:
	uint32_t	_allocated_size;
	uint32_t	_size;

public:
	dynamicArray(uint32_t init_size = 10) : _size(0), _allocated_size(0), data(nullptr)
	{
		data = (T*)malloc(sizeof(T) * init_size);
		_allocated_size = init_size;
	};
	~dynamicArray()
	{
		free(data);
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
			data = (T*)realloc(data, sizeof(T) *(size_t)_allocated_size);
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
};

//todo
//[] operaattori?
//copy construcrori?
//itaraattori?
//quicksort?
//resize vähentää kokoa?
//pidetäänkö push back reffinä?


template<typename T, const uint32_t poolSize = 50>
class pool
{
public:

private:
	uint32_t			_currentArraySize;
	dynamicArray<T*>	_data;
	dynamicArray<T*>	_freelist;
public:
	pool() :_currentArraySize(0) 
	{
		T* newdata = (T*)malloc(sizeof(T)* poolSize);
		_data.push_back(newdata);
	}
	~pool()
	{
		for (uint32_t i = 0; i < _data.get_size(); i++)
		{
			free(_data.data[i]);
		}
	}

	void insert(T& obj)
	{
		if (_freelist.get_size())
		{
			T** freeObj = _freelist.get_back();
			_freelist.delete_back();
			*(*freeObj) = obj;
			return;
		}

		if (_currentArraySize >= poolSize)
		{
			//do new array
			_currentArraySize = 0;
			T* newdata = (T*)malloc(sizeof(T)* poolSize);
			_data.push_back(newdata);
		}
		T** currentArray = _data.get_back();
		(*currentArray)[_currentArraySize++] = obj;
	}

	void delete_obj(uint32_t index)
	{
		uint32_t currentSize = _data.get_size() *poolSize;
		assert(currentSize > index);
		int poolindex = (uint32_t)((float)index / (float)poolSize);
		int arrayindex = index - poolindex;
		_freelist.push_back(_data.data[poolindex][arrayindex]);
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

