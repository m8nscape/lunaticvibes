#include "input_rawinput.h"

#if (_WIN32_WINNT >= 0x0501)
#include <hidusage.h>
#include <hidpi.h>


namespace Input::rawinput
{

RIMgr RIMgr::_inst;

int RIMgr::refreshDevices()
{
	std::unique_lock lock1(mutexList);
	std::unique_lock lock2(mutexInput);

	deviceInfo.clear();
	deviceKeyPressed.clear();
	deviceAxis.clear();
	deviceAxisDiff.clear();
	deviceHandleMap.clear();

	RAWINPUTDEVICELIST devs[MAX_DEVICE_COUNT] = { 0 };
	UINT count = MAX_DEVICE_COUNT;
	GetRawInputDeviceList(devs, &count, sizeof(RAWINPUTDEVICELIST));
	for (int i = 0; i < count; ++i)
	{
		if (devs[i].hDevice == NULL) continue;

		DeviceInfo devInfo;
		if (!getBasicDeviceinfo(devs[i].hDevice, devInfo))
			continue;

		if (!getJoystickDeviceInfo(devs[i].hDevice, devInfo))
			continue;

		int deviceID = (int)deviceInfo.size();
		deviceInfo.push_back(devInfo);
		deviceHandleMap[devs[i].hDevice] = deviceID;
		// insert map
		deviceKeyPressed[deviceID];
		deviceAxis[deviceID];
		deviceAxisDiff[deviceID];
	}
	LOG_INFO << "Rawinput: " << deviceInfo.size() << " devices detected";

	RAWINPUTDEVICE Rid[1];
	HWND hwnd;
	getWindowHandle(&hwnd);
	// HID
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_JOYSTICK;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = hwnd;
	// Do not register keyboard here. Joystick messages may behave like polling, thus other usages may lost randomly.
	// Mouse messages are handled by SDL2 with legacy messages
	RegisterRawInputDevices(Rid, sizeof(Rid) / sizeof(RAWINPUTDEVICE), sizeof(RAWINPUTDEVICE));

	return (int)deviceInfo.size();
}

bool RIMgr::getBasicDeviceinfo(HANDLE hDevice, DeviceInfo& devInfo)
{
	char devName[256] = { 0 };
	UINT devNameLen = 256;
	if ((int)GetRawInputDeviceInfoA(hDevice, RIDI_DEVICENAME, devName, &devNameLen) <= 0)
		return false;

	RID_DEVICE_INFO ridDevInfo = { 0 };
	UINT devInfoLen = sizeof(RID_DEVICE_INFO);
	if ((int)GetRawInputDeviceInfoA(hDevice, RIDI_DEVICEINFO, &ridDevInfo, &devInfoLen) <= 0)
		return false;

	devInfo.hidname = devName;
	devInfo.w32RidDevInfo = ridDevInfo;
	if (devName[0] != '\0')
	{
		HANDLE HIDHandle = CreateFile(devName, NULL, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		if (HIDHandle)
		{
			WCHAR prodStr[128] = { 0 };
			UINT prodStrLen = 128;
			HidD_GetProductString(HIDHandle, prodStr, prodStrLen);
			CloseHandle(HIDHandle);

			char ustr[512] = { 0 };
			WideCharToMultiByte(CP_UTF8, NULL, prodStr, prodStrLen, ustr, sizeof(ustr), NULL, FALSE);
			devInfo.productString = ustr;
		}
	}

	return true;
}


bool RIMgr::getJoystickDeviceInfo(HANDLE hDevice, DeviceInfo& devInfo)
{
	UINT bufferSize = 0;
	GetRawInputDeviceInfoA(hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize);
	if (bufferSize == 0) return false;

	devInfo.preparsedData.resize(bufferSize);
	PHIDP_PREPARSED_DATA preparsedData = (PHIDP_PREPARSED_DATA)devInfo.preparsedData.data();
	GetRawInputDeviceInfoA(hDevice, RIDI_PREPARSEDDATA, preparsedData, &bufferSize);

	HIDP_CAPS caps = { 0 };
	if (HidP_GetCaps(preparsedData, &caps) != HIDP_STATUS_SUCCESS) return false;

	// buttons
	devInfo.buttonCaps.resize(sizeof(HIDP_BUTTON_CAPS) * caps.NumberInputButtonCaps);
	PHIDP_BUTTON_CAPS btnCaps = (PHIDP_BUTTON_CAPS)devInfo.buttonCaps.data();
	USHORT capsLength = caps.NumberInputButtonCaps;
	if (HidP_GetButtonCaps(HidP_Input, btnCaps, &capsLength, preparsedData) == HIDP_STATUS_SUCCESS)
		devInfo.buttonCount = btnCaps->Range.UsageMax - btnCaps->Range.UsageMin + 1;

	// axis
	devInfo.valueCaps.resize(sizeof(HIDP_VALUE_CAPS) * caps.NumberInputValueCaps);
	PHIDP_VALUE_CAPS valCaps = (PHIDP_VALUE_CAPS)devInfo.valueCaps.data();
	capsLength = caps.NumberInputValueCaps;
	if (HidP_GetValueCaps(HidP_Input, valCaps, &capsLength, preparsedData) == HIDP_STATUS_SUCCESS)
		devInfo.axisCount = caps.NumberInputValueCaps;

	return true;
}


bool RIMgr::hasDevice(const std::string_view& hidname) const
{
	std::shared_lock lock(mutexList);

	int id = getDeviceID(hidname);
	return (id >= 0);
}

int RIMgr::getDeviceID(const std::string_view& hidname) const
{
	std::shared_lock lock(mutexList);

	for (int i = 0; i < (int)deviceInfo.size(); ++i)
	{
		if (deviceInfo[i].hidname == hidname) return i;
	}
	return -1;
}

DeviceInfo RIMgr::getDeviceInfo(const std::string_view& hidname) const
{
	std::shared_lock lock(mutexList);

	int id = getDeviceID(hidname);
	return (id >= 0) ? deviceInfo[id] : DeviceInfo();
}


// where is QWORD
typedef uint64_t QWORD;

LRESULT RIMgr::WMMsgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg != WM_INPUT) return 0;
	return inst()._WMMsgHandler(hwnd, msg, wParam, lParam);
}

LRESULT RIMgr::_WMMsgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg != WM_INPUT) return 0;

	// get one data from head
	HRAWINPUT hRI = (HRAWINPUT)lParam;
	UINT cbSize;
	if (0 == GetRawInputData(hRI, RID_INPUT, NULL, &cbSize, sizeof(RAWINPUTHEADER)))
	{
		assert(cbSize <= RAWINPUT_MSG_MAX_SIZE);
		RAWINPUT_MSG msg;
		if (-1 != GetRawInputData(hRI, RID_INPUT, &msg, &cbSize, sizeof(RAWINPUTHEADER)))
		{
			std::unique_lock lock(mutexList);
			msgList.push_back(msg);
		}
	}

	return 0;
}

void RIMgr::update()
{
	if (!msgList.empty())
	{
		std::list<RAWINPUT_MSG> msgListTmp;
		{
			std::unique_lock lock(mutexList);
			msgListTmp = msgList;
			msgList.clear();
		}

		for (auto& msg : msgListTmp)
		{
			PRAWINPUT ri = (PRAWINPUT)&msg;
			if (deviceHandleMap.find(ri->header.hDevice) == deviceHandleMap.end())
				continue;

			int deviceID = deviceHandleMap.at(ri->header.hDevice);
			if (deviceID < 0)
				continue;

			// joystick
			if (updateJoystick(ri)) continue;
		}
	}
}

bool RIMgr::updateJoystick(RAWINPUT* ri)
{
	if (ri->header.dwType != RIM_TYPEHID) return false;

	char buffer[2048];
	size_t bufferOffset = 0;
	auto Allocate = [&](size_t size) -> char* { if (size == 0) return nullptr; if (bufferOffset + size >= sizeof(buffer)) { panic("ERROR!", "updateJoystick alloc failed"); } char* p = &buffer[bufferOffset]; bufferOffset += size; return p; };

	int deviceID = deviceHandleMap.at(ri->header.hDevice);
	DeviceInfo& devInfo = deviceInfo[deviceID];

	PHIDP_PREPARSED_DATA preparsedData = (PHIDP_PREPARSED_DATA)devInfo.preparsedData.data();
	PHIDP_BUTTON_CAPS btnCaps = (PHIDP_BUTTON_CAPS)devInfo.buttonCaps.data();
	PHIDP_VALUE_CAPS valCaps = (PHIDP_VALUE_CAPS)devInfo.valueCaps.data();

	// buttons
	ULONG btnCount = devInfo.buttonCount;
	USAGE* btnUsage = (USAGE*)Allocate(sizeof(USAGE) * devInfo.buttonCount);
	if (devInfo.buttonCount > 0)
	{
		if (HidP_GetUsages(HidP_Input, btnCaps->UsagePage, 0, btnUsage, &btnCount, preparsedData, (PCHAR)ri->data.hid.bRawData, ri->data.hid.dwSizeHid) != HIDP_STATUS_SUCCESS) 
			btnCount = 0;
	}

	// axis
	using AXIS = std::pair<USAGE, ULONG>;
	bool hasAxis = devInfo.axisCount > 0;
	AXIS* axisVals = (AXIS*)Allocate((sizeof(AXIS) * devInfo.axisCount));
	for (int i = 0; i < devInfo.axisCount; i++)
	{
		ULONG value;
		USAGE axisIdx = valCaps[i].Range.UsageMin;
		axisVals[i].first = axisIdx;
		if (HidP_GetUsageValue(HidP_Input, valCaps[i].UsagePage, 0, axisIdx, &axisVals[i].second, preparsedData, (PCHAR)ri->data.hid.bRawData, ri->data.hid.dwSizeHid) != HIDP_STATUS_SUCCESS) 
			hasAxis = false;
	}

	{
		std::unique_lock lock(mutexInput);

		std::for_each(deviceKeyPressed[deviceID].begin(), deviceKeyPressed[deviceID].end(), [](decltype(*deviceKeyPressed[deviceID].begin()) p) { p.second = false; });
		if (btnCount > 0)
		{
			for (int i = 0; i < btnCount; ++i)
			{
				int btnIdx = btnUsage[i] - btnCaps->Range.UsageMin;
				deviceKeyPressed[deviceID][btnIdx] = true;
			}
		}

		if (hasAxis)
		{
			for (int i = 0; i < devInfo.axisCount; i++)
			{
				auto& [axisIdx, axisVal] = axisVals[i];
				deviceAxisDiff[deviceID][axisIdx] = (int)axisVal - deviceAxis[deviceID][axisIdx];
				deviceAxis[deviceID][axisIdx] = axisVal;
			}
		}
	}

	return true;
}

bool RIMgr::isPressed(int deviceID, int code) const 
{ 
	const auto& m = deviceKeyPressed.at(deviceID); 
	return m.find(code) != m.end() ? m.at(code) : false; 
}

int RIMgr::getAxisDiff(int deviceID, int idx) const 
{ 
	const auto& m = deviceAxisDiff.at(deviceID); 
	return m.find(idx) != m.end() ? m.at(idx) : 0; 
}

}

#endif