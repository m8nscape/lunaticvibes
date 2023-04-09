#ifdef WIN32

#include "input_dinput8.h"
#include "common/log.h"

InputDirectInput8::InputDirectInput8()
{
	HRESULT res = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8W, (LPVOID*)&lpdi, NULL);
	if (res != DI_OK)
	{
		LOG_ERROR << "[Input] DirectInput create error: " << res;
	}
}

InputDirectInput8::~InputDirectInput8()
{
	releaseDevices();
	deviceMouse = DeviceMouse();
	deviceKeyboard = DeviceKeyboard();
	deviceJoysticks.clear();
}

bool InputDirectInput8::acquireDevices()
{
	if (!lpdi)
	{
		LOG_ERROR << "[Input] DirectInput not created";
		return false;
	}

	if (deviceMouse.lpdid) deviceMouse.lpdid->Acquire();

	if (deviceKeyboard.lpdid) deviceKeyboard.lpdid->Acquire();

	for (auto& j : deviceJoysticks)
	{
		if (j.lpdid) j.lpdid->Acquire();
	}

	return true;
}

bool InputDirectInput8::releaseDevices()
{
	if (!lpdi)
	{
		return true;
	}

	if (deviceMouse.lpdid) deviceMouse.lpdid->Release();

	if (deviceKeyboard.lpdid) deviceKeyboard.lpdid->Release();

	for (auto& j : deviceJoysticks)
	{
		if (j.lpdid) j.lpdid->Release();
	}

	return true;
}


BOOL WDIEnumDevicesCallbackJoystick(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	return ((InputDirectInput8*)pvRef)->DIEnumDevicesCallbackJoystick(lpddi);
}

BOOL WDIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	return ((InputDirectInput8*)pvRef)->DIEnumDeviceObjectsCallback(lpddoi);
}


int InputDirectInput8::refreshDevices()
{
	if (!lpdi)
	{
		LOG_ERROR << "[Input] DirectInput not created";
		return 0;
	}

	releaseDevices();
	deviceMouse = DeviceMouse();
	deviceKeyboard = DeviceKeyboard();
	deviceJoysticks.clear();

	int count = 0;

	// create joysticks
	if (HRESULT hres = lpdi->EnumDevices(DI8DEVCLASS_GAMECTRL, WDIEnumDevicesCallbackJoystick, (LPVOID)this, DIEDFL_ATTACHEDONLY); hres == DI_OK)
	{
		count += deviceJoysticks.size();
	}
	else
	{
		LOG_ERROR << "[Input] DirectInput Enum Joystick error: " << hres;
	}

	// create keyboard
	if (HRESULT hres = lpdi->CreateDevice(GUID_SysKeyboard, &deviceKeyboard.lpdid, NULL); hres == DI_OK)
	{
		if (HRESULT hres1 = deviceKeyboard.lpdid->SetDataFormat(&c_dfDIKeyboard); hres1 == DI_OK)
		{
			++count;
		}
		else
		{
			LOG_WARNING << "[Input] DirectInput Keyboard SetDataFormat error: " << hres1;
		}
	}
	else
	{
		LOG_ERROR << "[Input] DirectInput Keyboard CreateDevice error: " << hres;
	}

	// create mouse
	if (HRESULT hres = lpdi->CreateDevice(GUID_SysMouse, &deviceMouse.lpdid, NULL); hres == DI_OK)
	{
		if (HRESULT hres1 = deviceMouse.lpdid->SetDataFormat(&c_dfDIMouse); hres1 == DI_OK)
		{
			++count;
		}
		else
		{
			LOG_WARNING << "[Input] DirectInput Mouse SetDataFormat error: " << hres1;
		}
	}
	else
	{
		LOG_ERROR << "[Input] DirectInput Mouse CreateDevice error: " << hres;
	}

	return count;
}

BOOL InputDirectInput8::DIEnumDevicesCallbackJoystick(LPCDIDEVICEINSTANCE lpddi)
{
	LPDIRECTINPUTDEVICE8  lpdiPad;

	if (HRESULT hres = lpdi->CreateDevice(lpddi->guidInstance, &lpdiPad, NULL); hres != DI_OK)
	{
		LOG_WARNING << "[Input] DirectInput Joystick CreateDevice error: " << hres;
		return DIENUM_CONTINUE;
	}
	if (HRESULT hres = lpdiPad->EnumObjects(WDIEnumDeviceObjectsCallback, NULL, DIDFT_ALL); hres != DI_OK)
	{
		LOG_WARNING << "[Input] DirectInput Joystick EnumObjects error: " << hres;
		return DIENUM_CONTINUE;
	}
	if (HRESULT hres = lpdiPad->SetDataFormat(&c_dfDIJoystick); hres != DI_OK)
	{
		LOG_WARNING << "[Input] DirectInput Joystick SetDataFormat error: " << hres;
		return DIENUM_CONTINUE;
	}
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_ABS;
	if (HRESULT hres = lpdiPad->SetProperty(DIPROP_AXISMODE, &dipdw.diph); hres != DI_OK)
	{
		LOG_WARNING << "[Input] DirectInput Joystick SetProperty DIPROPAXISMODE_ABS error: " << hres;
	}

	DeviceJoystick j;
	j.lpdid = lpdiPad;
	deviceJoysticks.push_back(j);

	return DIENUM_CONTINUE;
}

BOOL InputDirectInput8::DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi)
{

	return DIENUM_CONTINUE;
}

void InputDirectInput8::poll()
{
	if (!acquired)
	{
		acquireDevices();
	}

	if (lpdi)
	{
		if (deviceMouse.lpdid &&
			deviceMouse.lpdid->GetDeviceState(sizeof(deviceMouse.state), &deviceMouse.state) == DI_OK)
		{
		}
		else
		{
			deviceMouse.state = DIMOUSESTATE();
		}

		if (deviceKeyboard.lpdid &&
			deviceKeyboard.lpdid->GetDeviceState(sizeof(deviceKeyboard.state), deviceKeyboard.state) == DI_OK)
		{
		}
		else
		{
			memset(deviceKeyboard.state, 0, sizeof(deviceKeyboard.state));
		}

		for (auto& j : deviceJoysticks)
		{
			if (j.lpdid)
			{
				HRESULT hres = j.lpdid->Poll();
				if ((hres == DI_OK || hres == DI_NOEFFECT) &&
					j.lpdid->GetDeviceState(sizeof(j.state), &j.state) == DI_OK)
				{
				}
				else
				{
					j.state = DIJOYSTATE();
				}
			}
			else
			{
				j.state = DIJOYSTATE();
			}
		}
	}
}


size_t InputDirectInput8::getJoystickCount() const
{
	return deviceJoysticks.size();
}


const DIMOUSESTATE& InputDirectInput8::getMouseState() const
{
	return deviceMouse.state;
}

const BYTE* InputDirectInput8::getKeyboardState() const
{
	return deviceKeyboard.state;
}

const DIJOYSTATE& InputDirectInput8::getJoystickState(size_t idx) const
{
	assert(idx < deviceJoysticks.size());
	return deviceJoysticks[idx].state;
}

const InputDirectInput8::DeviceJoystick::Capabilities& InputDirectInput8::getJoystickCapabilities(size_t idx) const
{
	assert(idx < deviceJoysticks.size());
	return deviceJoysticks[idx].caps;
}

InputDirectInput8& InputDirectInput8::inst() 
{
	static InputDirectInput8 _inst; 
	return _inst; 
}

#endif
