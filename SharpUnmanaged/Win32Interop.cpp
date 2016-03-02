#include "stdafx.h"
#include <vcclr.h>
#include "Win32Interop.h"
#include "Win32Process.h"
#include <tlhelp32.h>

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

bool Win32Interop::VirtualFreeEx(IntPtr process, IntPtr regionAddress, int32_t size, FreeType freeType)
{
	return ::VirtualFreeEx(static_cast<HANDLE>(process), static_cast<LPVOID>(regionAddress), size, static_cast<DWORD>(freeType)) != NULL;
}

bool Win32Interop::GetThreadTimes(IntPtr handle, [Runtime::InteropServices::Out]DateTime ^%creationTime, [Runtime::InteropServices::Out]DateTime ^%exitTime, [Runtime::InteropServices::Out]DateTime ^%kernelTime, [Runtime::InteropServices::Out]DateTime ^%userTime)
{
	throw gcnew NotImplementedException();

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

array<int32_t>^ Win32Interop::GetThreads()
{
	System::Collections::Generic::List<int32_t>^ result = gcnew System::Collections::Generic::List<int32_t>();

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return result->ToArray();

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		::CloseHandle(hThreadSnap);
		return result->ToArray();
	}
	do
	{
		result->Add(te32.th32ThreadID);
	} while (Thread32Next(hThreadSnap, &te32));

	::CloseHandle(hThreadSnap);

	return result->ToArray();
}

array<int32_t>^ Win32Interop::GetThreads(int32_t processId)
{
	System::Collections::Generic::List<int32_t>^ result = gcnew System::Collections::Generic::List<int32_t>();

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return result->ToArray();

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		::CloseHandle(hThreadSnap);
		return result->ToArray();
	}
	do
	{
		if (te32.th32OwnerProcessID == processId)
		{
			result->Add(te32.th32ThreadID);
		}
	} while (Thread32Next(hThreadSnap, &te32));

	::CloseHandle(hThreadSnap);

	return result->ToArray();
}

int32_t Win32Interop::GetProcessIdFromThreadId(int32_t threadId)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return 0;

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		::CloseHandle(hThreadSnap);
		return 0;
	}
	do
	{
		if (te32.th32ThreadID == threadId)
		{
			return te32.th32OwnerProcessID;
		}
	} while (Thread32Next(hThreadSnap, &te32));

	::CloseHandle(hThreadSnap);

	return 0;
}