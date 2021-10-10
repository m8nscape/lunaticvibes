#pragma once
#include <array>
#include <shared_mutex>

template <class Key, class Value, size_t _size>
class buffered_global
{
public:
	buffered_global() : _data{ Value() }, _dataBuffer{ Value() }, _dataDefault{ Value() } { static_assert(_size > 0); }
	buffered_global(Value defVal) : buffered_global()
	{
		_data.fill(defVal);
		_dataBuffer.fill(defVal);
		_dataDefault.fill(defVal);
	}
private:
	mutable std::shared_mutex _mutex;
    std::array<Value, _size> _data;
    std::array<Value, _size> _dataBuffer;
	std::array<Value, _size> _dataDefault;

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

	bool queue(Key n, Value value)
	{
		size_t idx = (size_t)n;
		if (idx < _size)
		{
			_dataBuffer[idx] = value;
			return true;
		}
		return false;
	}

	[[deprecated]] bool set(Key n, Value value)
	{
		if (queue(n, value))
		{
			std::unique_lock lock(_mutex);
			size_t idx = (size_t)n;
			_data[idx] = _dataBuffer[idx];
			return true;
		}
		return false;
	}

	bool setDefault(Key n, Value value)
	{
		size_t idx = (size_t)n;
		if (idx < _size)
		{
			_dataDefault[idx] = value;
			return true;
		}
		return false;
	}

	void flush()
	{
		std::unique_lock lock(_mutex);
		_data = _dataBuffer; 
	}

	void reset() 
	{
		_dataBuffer = _dataDefault;
		flush();
	}

};

