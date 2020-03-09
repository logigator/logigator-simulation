#pragma once
#include <atomic>

template <class T>
class FastStackAtomic
{
public:
	FastStackAtomic() = default;
	
	FastStackAtomic(const FastStackAtomic &fs)
	{
		_expand(fs.capacity());
		_count = fs._count.load();

		for(size_t i = 0; i < fs._count.load(); i++)
		{
			_data[i] = fs._data[i];
		}
	}

	~FastStackAtomic()
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
		const auto temp = _count.fetch_add(1, std::memory_order_relaxed);
		if (temp >= _capacity)
			_expand();
		

		_data[temp] = item;
	}

	T pop()
	{
		if (_count.load() <= 0)
			return NULL;

		return _data[_count.fetch_sub(1, std::memory_order_relaxed) - 1];
	}

	void clear()
	{
		_count = 0;
	}
	
	bool empty() const
	{
		return _count.load() <= 0;
	}

	size_t count() const
	{
		return _count.load();
	}

	size_t capacity() const
	{
		return _capacity;
	}
private:
	T* _data = new T[2];
	std::atomic<size_t> _count = { 0 };
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

	void _expand(const size_t size)
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
