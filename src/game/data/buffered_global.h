#pragma once
#include <array>
#include <shared_mutex>

template <class Key, class Value, size_t _size>
class buffered_global
{
public:
	buffered_global() : _data{Value()}, _dataBuffer{Value()} { static_assert(_size > 0); }
private:
	mutable std::shared_mutex _mutex;
    std::array<Value, _size> _data;
    std::array<Value, _size> _dataBuffer;

public:
    Value get(Key n) const
    {
        size_t idx = (size_t)n;
		if (idx < _size)
		{
			std::shared_lock lock(_mutex);
			return _data.data()[idx];
		}
		else
		{
			return Value();
		}
	}

	[[deprecated]] void set(Key n, Value value)
	{
		size_t idx = (size_t)n;
		if (idx < _size)
		{
			std::unique_lock lock(_mutex);
			_data[idx] = _dataBuffer[idx] = value;
		}
	}

	void queue(Key n, Value value)
	{
		size_t idx = (size_t)n;
		if (idx < _size) 
			_dataBuffer[idx] = value;
	}

	void flush()
	{
		std::unique_lock lock(_mutex);
		_data = _dataBuffer; 
	}

	void reset() 
	{
		_dataBuffer = { 0 };
		flush();
	}

};

