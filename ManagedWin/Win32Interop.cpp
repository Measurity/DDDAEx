#include "stdafx.h"
#include "Win32Interop.h"

using namespace SharpUnmanaged;

WaitForType Win32Interop::WaitForSingleObject(IntPtr handle, int32_t ms)
{
	return static_cast<WaitForType>(::WaitForSingleObject(static_cast<HANDLE>(handle), ms));
}

WaitForType Win32Interop::WaitForSingleObjectInfinite(IntPtr handle)
{
	return Win32Interop::WaitForSingleObject(handle, INFINITE);
}

bool Win32Interop::CloseHandle(IntPtr handle)
{
	return ::CloseHandle(static_cast<HANDLE>(handle)) != NULL;
}

IntPtr Win32Interop::Alloc(IntPtr process, IntPtr address, int size, AllocationType allocation, ProtectType protect)
{
	return static_cast<IntPtr>(::VirtualAllocEx(static_cast<HANDLE>(process), static_cast<LPVOID>(address), size, static_cast<DWORD>(allocation), static_cast<DWORD>(protect)));
}

IntPtr Win32Interop::CreateRemoteThread(IntPtr process, IntPtr entryAddress)
{
	return Win32Interop::CreateRemoteThread(process, entryAddress, IntPtr::Zero);
}

IntPtr Win32Interop::CreateRemoteThread(IntPtr process, IntPtr entryAddress, IntPtr argumentAddress)
{
	return static_cast<IntPtr>(::CreateRemoteThread(static_cast<HANDLE>(process), NULL, NULL, static_cast<LPTHREAD_START_ROUTINE>(static_cast<LPVOID>(entryAddress)), static_cast<LPVOID>(argumentAddress), NULL, NULL));
}

IntPtr Win32Interop::GetCurrentProcess()
{
	return static_cast<IntPtr>(::GetCurrentProcess());
}

bool Win32Interop::VirtualFreeEx(IntPtr process, IntPtr regionAddress, int32_t size, FreeType freeType)
{
	return ::VirtualFreeEx(static_cast<HANDLE>(process), static_cast<LPVOID>(regionAddress), size, static_cast<DWORD>(freeType)) != NULL;
}