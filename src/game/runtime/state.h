#pragma once

#include "index/bargraph.h"
#include "index/number.h"
#include "index/option.h"
#include "index/slider.h"
#include "index/switch.h"
#include "index/text.h"
#include "index/timer.h"

// Global state value manager
class State
{
private:
	static State _inst;

protected:
	template <class Key, class Value, size_t _size>
	class StateContainer
	{
	public:
		using KeyType = Key;
		using ValType = Value;

	public:
		StateContainer() : _data{ Value() }, _dataDefault{ Value() } { static_assert(_size > 0); }
		StateContainer(Value defVal) : StateContainer()
		{
			_data.fill(defVal);
			_dataDefault.fill(defVal);
		}
	private:
		std::array<Value, _size> _data;
		std::array<Value, _size> _dataDefault;

	public:
		Value get(Key n) const
		{
			size_t idx = (size_t)n;
			if (idx < _size)
			{
				return _data.data()[idx];
			}
			return Value();
		}

		bool set(Key n, Value value)
		{
			size_t idx = (size_t)n;
			if (idx < _size)
			{
				_data[idx] = value;
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

		void reset()
		{
			_data = _dataDefault;
		}
	};
	StateContainer<IndexBargraph, Ratio, (size_t)IndexBargraph::BARGRAPH_COUNT> gBargraphs;
	StateContainer<IndexNumber, int, (size_t)IndexNumber::NUMBER_COUNT> gNumbers;
	StateContainer<IndexOption, unsigned, (size_t)IndexOption::OPTION_COUNT> gOptions;
	StateContainer<IndexSlider, Ratio, (size_t)IndexSlider::SLIDER_COUNT> gSliders;
	StateContainer<IndexSwitch, bool, (size_t)IndexSwitch::SWITCH_COUNT> gSwitches;
	StateContainer<IndexText, std::string, (size_t)IndexText::TEXT_COUNT> gTexts;
	StateContainer<IndexTimer, long long, (size_t)IndexTimer::TIMER_COUNT> gTimers{ TIMER_NEVER };

private:
	State();

public:
	static bool set(IndexBargraph ind, Ratio val);
	static double get(IndexBargraph ind);

	static bool set(IndexNumber ind, int val);
	static int get(IndexNumber ind);

	static bool set(IndexOption ind, unsigned val);
	static unsigned get(IndexOption ind);

	static bool set(IndexSlider ind, Ratio val);
	static double get(IndexSlider ind);

	static bool set(IndexSwitch ind, bool val);
	static bool get(IndexSwitch ind);

	static bool set(IndexText ind, std::string_view val);
	static std::string get(IndexText ind);

	static bool set(IndexTimer ind, long long val);
	static long long get(IndexTimer ind);
	static void resetTimer();
};