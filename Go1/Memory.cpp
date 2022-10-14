#include "Memory.h"

Memory::~Memory()
{
	::CloseHandle(hProcess);
}

bool Memory::Init(LPCTSTR processName)
{
	processId = GetProcessId(processName);
	if (processId == 0)
	{
		::wprintf_s(L"找不到进程\n");
		return false;
	}
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		::wprintf_s(L"句柄不能打开\n");
		return false;
	}
	return true;
}

uint32_t Memory::GetProcessId(LPCTSTR processName)
{
	uint32_t processId = 0;
	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process{ 0 };
	process.dwSize = sizeof(process);
	while (::Process32Next(hSnap, &process))
	{
		if (::_wcsicmp(process.szExeFile, processName) == 0)
		{
			processId = process.th32ProcessID;
			break;
		}
	}
	::CloseHandle(hSnap);
	return processId;
}

uintptr_t Memory::GetModuleAddress(LPCTSTR moduleName)
{
	uintptr_t moduleAddress = 0;
	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	MODULEENTRY32 module;
	module.dwSize = sizeof(module);
	while (::Module32Next(hSnap, &module))
	{
		if (::_wcsicmp(module.szModule, moduleName) == 0)
		{
			moduleAddress = reinterpret_cast<uintptr_t>(module.modBaseAddr);
			break;
		}
	}
	::CloseHandle(hSnap);
	return moduleAddress;
}

bool Memory::ReadBuffer(uintptr_t address, void* buffer, size_t size)
{
	SIZE_T bytes = 0;
	::ReadProcessMemory(hProcess, reinterpret_cast<void*>(address), buffer, size, &bytes);
	if (bytes == 0)
	{
		::wprintf_s(L"读取失败\n");
		return false;
	}
	return true;
}
