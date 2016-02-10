// This is the main DLL file.

#include "stdafx.h"
#include "ManagedWin.h"
#include <iostream>
#include <vcclr.h>
#include <msclr\marshal_cppstd.h>

System::IntPtr ManagedWin::Win32Process::OpenProcess(ProcessAccess access, int pid)
{
	return static_cast<System::IntPtr>(::OpenProcess(static_cast<DWORD>(access), FALSE, static_cast<DWORD>(pid)));
}

System::IntPtr ManagedWin::Win32Process::GetModuleHandle(System::String^ moduleName)
{
	pin_ptr<const wchar_t> wModuleName = PtrToStringChars(moduleName);
	return static_cast<System::IntPtr>(::GetModuleHandleW(wModuleName));
}

System::IntPtr ManagedWin::Win32Process::GetProcAddress(System::IntPtr module, System::String^ functionName)
{
	const char* cFunctionName = (char*)Marshal::StringToHGlobalAnsi(functionName).ToPointer();
	return static_cast<System::IntPtr>(::GetProcAddress(static_cast<HMODULE>(static_cast<HANDLE>(module)), cFunctionName));
}

System::IntPtr ManagedWin::Win32Process::Execute(System::IntPtr handle, System::IntPtr entryAddress, System::IntPtr argumentAddress)
{
	return static_cast<System::IntPtr>(::CreateRemoteThread(static_cast<HANDLE>(handle), NULL, NULL, static_cast<LPTHREAD_START_ROUTINE>(static_cast<LPVOID>(entryAddress)), static_cast<LPVOID>(argumentAddress), NULL, NULL));
}

System::IntPtr ManagedWin::Win32Process::Alloc(System::IntPtr process, System::IntPtr address, int size, AllocationType allocation, ProtectType protect)
{
	return static_cast<System::IntPtr>(::VirtualAllocEx(static_cast<HANDLE>(process), static_cast<LPVOID>(address), size, static_cast<DWORD>(allocation), static_cast<DWORD>(protect)));
}

System::IntPtr ManagedWin::Win32Process::Alloc(System::IntPtr process, System::IntPtr address, unsigned long size, AllocationType allocation, ProtectType protect)
{
	return static_cast<System::IntPtr>(::VirtualAllocEx(static_cast<HANDLE>(process), static_cast<LPVOID>(address), size, static_cast<DWORD>(allocation), static_cast<DWORD>(protect)));
}

System::IntPtr ManagedWin::Win32Process::Load(System::String^ dll)
{
	pin_ptr<const wchar_t> wDll = PtrToStringChars(dll);
	return static_cast<System::IntPtr>(LoadLibrary(wDll));
}

ManagedWin::WaitForType ManagedWin::Win32Process::WaitInfinite(System::IntPtr handle)
{
	return static_cast<ManagedWin::WaitForType>(::WaitForSingleObject(static_cast<HANDLE>(handle), INFINITE));
}

System::IntPtr ManagedWin::Win32Process::GetExitCodeThread(System::IntPtr threadHandle)
{
	DWORD exit;
	::GetExitCodeThread(static_cast<HANDLE>(threadHandle), &exit);
	return static_cast<System::IntPtr>((int)exit);
}

bool ManagedWin::Win32Process::Free(System::IntPtr process, System::IntPtr address)
{
	return ::VirtualFreeEx(static_cast<HANDLE>(process), static_cast<LPVOID>(address), NULL, static_cast<DWORD>(FreeType::MemRelease)) != NULL;
}

bool ManagedWin::Win32Process::Free(System::IntPtr process, System::IntPtr address, unsigned long size, FreeType freeType)
{
	return ::VirtualFreeEx(static_cast<HANDLE>(process), static_cast<LPVOID>(address), size, static_cast<DWORD>(freeType)) != NULL;
}

uint32_t ManagedWin::Win32Process::WriteString(System::IntPtr handle, System::IntPtr address, System::String^ value)
{
	const char* buffer = (const char*)(Marshal::StringToHGlobalAnsi(value)).ToPointer();
	SIZE_T bytesWritten = 0;
	::WriteProcessMemory(static_cast<HANDLE>(handle), static_cast<LPVOID>(address), buffer, strlen(buffer), &bytesWritten);
	return bytesWritten;
}

void ManagedWin::Win32Process::CloseHandle(System::IntPtr handle)
{
	::CloseHandle(static_cast<HANDLE>(handle));
}

int32_t ManagedWin::Win32Process::ReadInt(System::IntPtr handle, System::IntPtr address)
{
	int32_t value;
	::ReadProcessMemory(static_cast<HANDLE>(handle), static_cast<LPCVOID>(address), &value, sizeof(value), NULL);
	return value;
}