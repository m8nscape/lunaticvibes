#include "input_rawinput.h"

#if (_WIN32_WINNT >= 0x0501)


namespace Input::rawinput
{

RIMgr RIMgr::_inst;

int RIMgr::refreshDevices()
{
	std::unique_lock lock(mt);

	deviceInfo.clear();
	deviceKeyPressed.clear();
	deviceHandleMap.clear();

	RAWINPUTDEVICELIST devs[MAX_DEVICE_COUNT] = { 0 };
	UINT count = MAX_DEVICE_COUNT;
	GetRawInputDeviceList(devs, &count, sizeof(RAWINPUTDEVICELIST));

	for (int i = 0; i < count; ++i)
	{
		if (devs[i].hDevice == NULL) continue;

		char devName[256] = { 0 };
		UINT devNameLen = 256;
		UINT ret = GetRawInputDeviceInfoA(devs[i].hDevice, RIDI_DEVICENAME, devName, &devNameLen);

		RID_DEVICE_INFO ridDevInfo = { 0 };
		UINT devInfoLen = sizeof(RID_DEVICE_INFO);
		UINT ret1 = GetRawInputDeviceInfoA(devs[i].hDevice, RIDI_DEVICEINFO, &ridDevInfo, &devInfoLen);

		DeviceInfo devInfo;
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

		deviceInfo.push_back(devInfo);
		deviceHandleMap[devs[i].hDevice] = (int)deviceInfo.size();
	}
	return (int)deviceInfo.size();
}

bool RIMgr::hasDevice(const std::string_view& hidname) const
{
	std::shared_lock lock(mt);

	int id = getDeviceID(hidname);
	return (id >= 0);
}

int RIMgr::getDeviceID(const std::string_view& hidname) const
{
	std::shared_lock lock(mt);

	for (int i = 0; i < (int)deviceInfo.size(); ++i)
	{
		if (deviceInfo[i].hidname == hidname) return i;
	}
	return -1;
}

DeviceInfo RIMgr::getDeviceInfo(const std::string_view& hidname) const
{
	std::shared_lock lock(mt);

	int id = getDeviceID(hidname);
	return (id >= 0) ? deviceInfo[id] : DeviceInfo();
}


// where is QWORD
typedef uint64_t QWORD;

void RIMgr::WMMsgHandler(void* arg1, void* arg2, void* arg3, void* arg4)
{
	HWND hwnd = *(HWND*)arg1;
	UINT msg = *(UINT*)arg2;
	WPARAM wParam = *(WPARAM*)arg3;
	LPARAM lParam = *(LPARAM*)arg4;

	if (msg != WM_INPUT) return;


	UINT cbSize;

	// get one data from head
	HRAWINPUT hRI = (HRAWINPUT)lParam;
	static std::list<RAWINPUT> msgList;
	if (0 == GetRawInputData(hRI, RID_INPUT, NULL, &cbSize, sizeof(RAWINPUTHEADER)))
	{
		assert(cbSize <= sizeof(RAWINPUT));
		auto pCb = std::reinterpret_pointer_cast<RAWINPUT>(std::shared_ptr<char[]>(new char[cbSize]));
		if (-1 != GetRawInputData(hRI, RID_INPUT, &*pCb, &cbSize, sizeof(RAWINPUTHEADER)))
		{
			msgList.push_back(*pCb);
		}
	}

	// also read buffer if appliciable
	if (GetRawInputBuffer(NULL, &cbSize, sizeof(RAWINPUTHEADER)) != 0)
	{
		cbSize *= 16; // up to 16 messages
		auto pRawInput = std::reinterpret_pointer_cast<RAWINPUT>(std::shared_ptr<char[]>(new char[cbSize]));
		if (pRawInput != nullptr)
		{
			for (;;)
			{
				UINT cbSizeT = cbSize;
				UINT nInput = GetRawInputBuffer(&*pRawInput, &cbSizeT, sizeof(RAWINPUTHEADER));
				if (nInput <= 0) break;

				PRAWINPUT pri = pRawInput.get();
				for (UINT i = 0; i < nInput; ++i)
				{
					msgList.push_back(*pri);
					pri = NEXTRAWINPUTBLOCK(pri);
				}
			}
		}
	}

	// buffer data
	if (!msgList.empty())
	{
		std::unique_lock lock(mt);
		msgListQueue.push_back(msgList);
	}

	msgList.clear();

}

void RIMgr::update()
{
	while (!msgListQueue.empty())
	{
		std::list<RAWINPUT> msgList;
		{
			std::unique_lock lock(mt);
			if (msgListQueue.empty())
			{
				assert(false);
				break;
			}
			msgList = msgListQueue.front();
			msgListQueue.pop_front();
		}

		for (auto& ri : msgList)
		{
			if (deviceHandleMap.find(ri.header.hDevice) == deviceHandleMap.end())
				continue;

			int deviceID = deviceHandleMap.at(ri.header.hDevice);
			if (deviceID < 0)
				continue;

			switch (ri.header.dwType)
			{
			case RIM_TYPEMOUSE:
				if (ri.data.mouse.usButtonFlags)
				{
					USHORT flags = ri.data.mouse.usButtonFlags;
					if (flags & RI_MOUSE_BUTTON_1_DOWN) deviceKeyPressed[deviceID][0] = true;
					if (flags & RI_MOUSE_BUTTON_1_UP)   deviceKeyPressed[deviceID][0] = false;
					if (flags & RI_MOUSE_BUTTON_2_DOWN) deviceKeyPressed[deviceID][1] = true;
					if (flags & RI_MOUSE_BUTTON_2_UP)   deviceKeyPressed[deviceID][1] = false;
					if (flags & RI_MOUSE_BUTTON_3_DOWN) deviceKeyPressed[deviceID][2] = true;
					if (flags & RI_MOUSE_BUTTON_3_UP)   deviceKeyPressed[deviceID][2] = false;
					if (flags & RI_MOUSE_BUTTON_4_DOWN) deviceKeyPressed[deviceID][3] = true;
					if (flags & RI_MOUSE_BUTTON_4_UP)   deviceKeyPressed[deviceID][3] = false;
					if (flags & RI_MOUSE_BUTTON_5_DOWN) deviceKeyPressed[deviceID][4] = true;
					if (flags & RI_MOUSE_BUTTON_5_UP)   deviceKeyPressed[deviceID][4] = false;
				}
				if (ri.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
				{

				}
				if (ri.data.mouse.usButtonFlags & (RI_MOUSE_WHEEL | RI_MOUSE_HWHEEL))
				{

				}
				break;

			case RIM_TYPEKEYBOARD:
				deviceKeyPressed[deviceID][ri.data.keyboard.VKey] = (ri.data.keyboard.Flags & 1) == 0;
				break;

			case RIM_TYPEHID:
				break;
			default:
				break;
			}
		}
	}
}


}

#endif