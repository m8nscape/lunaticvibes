#pragma once

#if (_WIN32_WINNT >= 0x0501)
#define RAWINPUT_AVAILABLE

// defines
#include <WinUser.h>

// structs
#include <hidusage.h>

// HidD_GetProductString
#include <hidsdi.h>
#pragma comment(lib, "hid.lib")

#include <shared_mutex>

namespace Input::rawinput
{

struct DeviceInfo
{
	std::string hidname;		// \\?\HID#VID_046D&PID_C084&MI_01&Col05#7&31497a12&0&0004#{4d1e55b2-f16f-11cf-88cb-001111000030}
	std::string productString;	// G102 Prodigy Gaming Mouse
	RID_DEVICE_INFO w32RidDevInfo = { sizeof(RID_DEVICE_INFO) };
};

class RIMgr
{
private:
	static RIMgr _inst;
	RIMgr() = default;
	~RIMgr() = default;
public:
	static RIMgr& inst() { return _inst; }

protected:
	mutable std::shared_mutex mt;
	std::vector<DeviceInfo> deviceInfo;
	std::map<HANDLE, int> deviceHandleMap;
	std::list<std::list<RAWINPUT>> msgListQueue;
	std::map<int, std::map<int, bool>> deviceKeyPressed;
public:
	static constexpr size_t MAX_DEVICE_COUNT = 32;

public:
	int refreshDevices();
	bool hasDevice(const std::string_view& hidname) const;
	int getDeviceID(const std::string_view& hidname) const;
	DeviceInfo getDeviceInfo(const std::string_view& hidname) const;

	void WMMsgHandler(void*, void*, void*, void*);
	void update();

	bool isPressed(int deviceID, int code) { return deviceKeyPressed[deviceID][code];}
};

}

#endif