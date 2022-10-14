#pragma once
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include <TlHelp32.h>

class Memory
{
public:
	~Memory();
	bool Init(LPCTSTR processName);
	uint32_t GetProcessId(LPCTSTR processName);
	uintptr_t GetModuleAddress(LPCTSTR moduleName);
	template<class T>
	T Read(uintptr_t address);
	template<class T>
	bool Write(uintptr_t address, T buffer);
	bool ReadBuffer(uintptr_t address, void* buffer, size_t size);
public:
	uint32_t processId = 0;
	HANDLE hProcess = 0;
};

template<class T>
inline T Memory::Read(uintptr_t address)
{
	T buffer;
	SIZE_T size = 0;
	::ReadProcessMemory(hProcess, reinterpret_cast<void*>(address), &buffer, sizeof(buffer), &size);
	if (size == 0)
	{
		::wprintf_s(L"∂¡»° ß∞‹\n");
	}
	return buffer;
}

template<class T>
inline bool Memory::Write(uintptr_t address, T buffer)
{
	SIZE_T size = 0;
	::WriteProcessMemory(hProcess, reinterpret_cast<void*>(address), &buffer, sizeof(buffer), &size);
	if (size == 0)
	{
		::wprintf_s(L"–¥»Î ß∞‹\n");
		return false;
	}
	return true;
}
