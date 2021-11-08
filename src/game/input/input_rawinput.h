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

class InputMgr;
namespace Input::rawinput
{

struct DeviceInfo
{
	std::string hidname;		// \\?\HID#VID_046D&PID_C084&MI_01&Col05#7&31497a12&0&0004#{4d1e55b2-f16f-11cf-88cb-001111000030}
	std::string productString;	// G102 Prodigy Gaming Mouse
	RID_DEVICE_INFO w32RidDevInfo = { sizeof(RID_DEVICE_INFO) };

	// Joystick
	size_t buttonCount = 0;
	size_t axisCount = 0;

	// internal data
	std::string preparsedData;
	std::string buttonCaps;
	std::string valueCaps;
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
	friend class ::InputMgr;
	mutable std::shared_mutex mutexList;
	mutable std::shared_mutex mutexInput;
	std::vector<DeviceInfo> deviceInfo;
	std::map<HANDLE, int> deviceHandleMap;
	std::map<HANDLE, std::shared_ptr<char[]>> devicePreParsedData;

	static constexpr size_t RAWINPUT_MSG_MAX_SIZE = 256;
	struct RAWINPUT_MSG { char data[RAWINPUT_MSG_MAX_SIZE]; };
	std::list<RAWINPUT_MSG> msgList;

	std::map<int, std::map<int, bool>>  deviceKeyPressed;
	std::map<int, std::map<int, ULONG>> deviceAxis;
	std::map<int, std::map<int, int>> deviceAxisDiff;

	bool getBasicDeviceinfo(HANDLE hDevice, DeviceInfo& devInfo);
	bool getJoystickDeviceInfo(HANDLE hDevice, DeviceInfo& devInfo);
	LRESULT _WMMsgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	static constexpr size_t MAX_DEVICE_COUNT = 32;

public:
	int refreshDevices();
	bool hasDevice(const std::string_view& hidname) const;
	size_t getDeviceCount() const { return deviceInfo.size(); }
	int getDeviceID(const std::string_view& hidname) const;
	DeviceInfo getDeviceInfo(const std::string_view& hidname) const;

	static LRESULT WMMsgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void update();
	bool updateJoystick(RAWINPUT* ri);

	auto getPressed(int deviceID) const { return deviceKeyPressed.at(deviceID); }
	auto getAxisDiff(int deviceID) const { return deviceAxisDiff.at(deviceID); }
	bool isPressed(int deviceID, int code) const;
	int getAxisDiff(int deviceID, int idx) const;
};

}

#endif