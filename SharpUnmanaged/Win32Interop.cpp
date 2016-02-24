#include "stdafx.h"
#include <vcclr.h>
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

bool Win32Interop::GetThreadTimes(IntPtr handle, [Runtime::InteropServices::Out]DateTime^% creationTime, [Runtime::InteropServices::Out]DateTime^% exitTime, [Runtime::InteropServices::Out]DateTime^% kernelTime, [Runtime::InteropServices::Out]DateTime^% userTime)
{
	_FILETIME ct, et, kt, ut;
	BOOL result = ::GetThreadTimes(static_cast<HANDLE>(handle), &ct, &et, &kt, &ut);
	if (result == FALSE) return false;

	creationTime = DateTime::FromFileTime((static_cast<int64_t>(ct.dwHighDateTime) << 32) | ct.dwLowDateTime);
	exitTime = DateTime::FromFileTime((static_cast<int64_t>(et.dwHighDateTime) << 32) | et.dwLowDateTime);
	kernelTime = DateTime::FromFileTime((static_cast<int64_t>(kt.dwHighDateTime) << 32) | kt.dwLowDateTime);
	userTime = DateTime::FromFileTime((static_cast<int64_t>(ut.dwHighDateTime) << 32) | ut.dwLowDateTime);
	return true;
}

int32_t Win32Interop::GetProcessId(IntPtr handle)
{
	return ::GetProcessId(static_cast<HANDLE>(handle));
}

String^ Win32Interop::GetModuleFileEx(IntPtr process, IntPtr module)
{
	return Win32Interop::GetModuleFileEx(process, module, MAX_PATH);
}

String^ Win32Interop::GetModuleFileEx(IntPtr process, IntPtr module, int32_t size)
{
	TCHAR result[_MAX_FNAME];
	::GetModuleFileNameEx(static_cast<HANDLE>(process), static_cast<HMODULE>(static_cast<HANDLE>(module)), result, size);
	return gcnew String(result);
}

ModuleInfo Win32Interop::GetModuleInfo(IntPtr process, IntPtr module)
{
	MODULEINFO mi;
	::GetModuleInformation(static_cast<HANDLE>(process), static_cast<HMODULE>(static_cast<HANDLE>(module)), &mi, sizeof(mi));

	ModuleInfo i;
	i.BaseAddress = static_cast<IntPtr>(mi.lpBaseOfDll);
	i.EntryPoint = static_cast<IntPtr>(mi.EntryPoint);
	i.SizeOfImage = static_cast<int32_t>(mi.SizeOfImage);
	return i;
}

String^ Win32Interop::GetModuleName(IntPtr process, IntPtr module)
{
	TCHAR result[_MAX_FNAME];
	::GetModuleBaseName(static_cast<HANDLE>(process), static_cast<HMODULE>(static_cast<HANDLE>(module)), result, _MAX_FNAME);
	return gcnew String(result);
}

IntPtr Win32Interop::GetProcAddress(IntPtr module, String^ exportedProcName)
{
	IntPtr funcName = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(exportedProcName);
	return static_cast<IntPtr>(::GetProcAddress(static_cast<HMODULE>(static_cast<HANDLE>(module)), (LPCSTR)funcName.ToPointer()));
}