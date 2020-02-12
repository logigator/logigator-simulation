#pragma once
#include <atomic>

template <class T>
class FastStack
{
public:
	FastStack() = default;
	
	FastStack(const FastStack &fs)
	{
		_expand(fs.capacity());
		_count = fs._count;

		for(size_t i = 0; i < fs._count; i++)
		{
			_data[i] = fs._data[i];
		}
	}

	~FastStack()
	{
		delete[] _data;
	}

	T& operator[](int index)
	{
		return _data[index];
	}
	
	const T& operator[](int index) const
	{
		return _data[index];
	}

	void push(T item)
	{
		if (++_count > _capacity)
			_expand();

		_data[_count - 1] = item;
	}

	T pop()
	{
		if (_count <= 0)
			return NULL;

		return _data[--_count];
	}

	void clear()
	{
		_count = 0;
	}

	bool empty()
	{
		return _count <= 0;
	}

	size_t count()
	{
		return _count;
	}

	size_t capacity()
	{
		return _capacity;
	}
private:
	T* _data = new T[2];
	size_t _count = 0;
	size_t _capacity = 2;

	void _expand()
	{
		T* allocation = new T[_capacity * 2];
		for(size_t i = 0; i < _capacity; i++)
		{
			allocation[i] = _data[i];
		}
		delete[] _data;
		_data = allocation;
		_capacity *= 2;
	}

	void _expand(size_t size)
	{
		T* allocation = new T[size];
		for (size_t i = 0; i < _capacity; i++)
		{
			allocation[i] = _data[i];
		}
		delete[] _data;
		_data = allocation;
		_capacity = size;
	}
};
