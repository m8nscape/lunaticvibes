#include "common/pch.h"
#include "state.h"

State State::_inst;

State::State()
{
	gNumbers.setDefault(IndexNumber::ZERO, 0);
	gSwitches.setDefault(IndexSwitch::_FALSE, false);
	gSwitches.setDefault(IndexSwitch::_TRUE, true);
	gSliders.setDefault(IndexSlider::ZERO, 0);
	gTexts.setDefault(IndexText::INVALID, "");
	gTimers.setDefault(IndexTimer::_NEVER, TIMER_NEVER);

	gNumbers.reset();
	gSwitches.reset();
	gSliders.reset();
	gTexts.reset();
	gTimers.reset();
}

bool State::set(IndexBargraph ind, Ratio val)
{
	return _inst.gBargraphs.set(ind, val);
}

double State::get(IndexBargraph ind)
{
	return _inst.gBargraphs.get(ind);
}

bool State::set(IndexNumber ind, int val)
{
	return _inst.gNumbers.set(ind, val);
}

int State::get(IndexNumber ind)
{
	return _inst.gNumbers.get(ind);
}


bool State::set(IndexOption ind, unsigned val)
{
	return _inst.gOptions.set(ind, val);
}

unsigned State::get(IndexOption ind)
{
	return _inst.gOptions.get(ind);
}


bool State::set(IndexSlider ind, Ratio val)
{
	return _inst.gSliders.set(ind, val);
}

double State::get(IndexSlider ind)
{
	return _inst.gSliders.get(ind);
}


bool State::set(IndexSwitch ind, bool val)
{
	return _inst.gSwitches.set(ind, val);
}

bool State::get(IndexSwitch ind)
{
	return _inst.gSwitches.get(ind);
}


bool State::set(IndexText ind, std::string_view val)
{
	return _inst.gTexts.set(ind, std::string(val));
}

std::string State::get(IndexText ind)
{
	return _inst.gTexts.get(ind);
}


bool State::set(IndexTimer ind, long long val)
{
	return _inst.gTimers.set(ind, val);
}

long long State::get(IndexTimer ind)
{
	return _inst.gTimers.get(ind);
}

void State::resetTimer()
{
	long long customizeTimer = get(IndexTimer::_SCENE_CUSTOMIZE_START);
	_inst.gTimers.reset();
	set(IndexTimer::_SCENE_CUSTOMIZE_START, customizeTimer);
}
