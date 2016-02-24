// This is the main DLL file.

#include "stdafx.h"
#include "Win32Process.h"
#include <iostream>
#include <vcclr.h>
#include <tlhelp32.h>
#include <msclr\marshal_cppstd.h>
#include "Win32Interop.h"

using namespace System;
using namespace SharpUnmanaged;

#pragma region De/Con- structors

Win32Process::Win32Process(int32_t id)
{
	Win32Process(id, ProcessAccess::All);
}

Win32Process::Win32Process(int32_t id, ProcessAccess access)
{
	if (id <= 0) throw gcnew ArgumentException("Id should be higher than 0.");
	Handle = static_cast<IntPtr>(::OpenProcess(static_cast<DWORD>(access), FALSE, id));
}

Win32Process::Win32Process(IntPtr handle)
{
	Win32Process(handle, ProcessAccess::All);
}

Win32Process::Win32Process(IntPtr handle, ProcessAccess access)
{
	if (handle == IntPtr::Zero) throw gcnew Exception("Could not open the process with all access rights. Make sure you're program is executing with sufficient rights.");
	Id = ::GetProcessId(static_cast<HANDLE>(handle));
	if (Id <= 0) throw gcnew Exception("Could not get process id from handle.");
	Handle = handle;
}

Win32Process::~Win32Process()
{
	::CloseHandle(static_cast<HANDLE>(Handle));
}

Win32Process^ Win32Process::Current()
{
	return gcnew Win32Process(Win32Interop::GetCurrentProcess());
}

Win32Process^ Win32Process::Create(String^ fileName, ProcessCreationType creationType)
{
	pin_ptr<const wchar_t> wFile = PtrToStringChars(fileName);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	::ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	::ZeroMemory(&pi, sizeof(pi));

	CreateProcessW(wFile, NULL, NULL, NULL, FALSE, static_cast<DWORD>(creationType), NULL, NULL, &si, &pi);

	::CloseHandle(pi.hThread);
	return gcnew Win32Process(static_cast<IntPtr>(pi.hProcess));
}

Win32Process^ Win32Process::Create(String^ fileName)
{
	return Create(fileName, ProcessCreationType::Default);
}

#pragma endregion

#pragma region public members

System::IntPtr Win32Process::GetModuleHandle(System::String^ moduleName)
{
	pin_ptr<const wchar_t> wModuleName = PtrToStringChars(moduleName);
	return static_cast<System::IntPtr>(::GetModuleHandleW(wModuleName));
}

array<Byte>^ Win32Process::ReadBytes(IntPtr address, uint32_t size)
{
	uint8_t* buffer = new uint8_t[size];
	ReadProcessMemory(static_cast<HANDLE>(this->Handle), static_cast<LPVOID>(address), buffer, size, NULL);

	array<Byte>^ result = gcnew array<Byte>(size);
	pin_ptr<Byte> result_start = &result[0];
	memcpy(result_start, buffer, size);

	return result;
}

int32_t Win32Process::ReadInt(System::IntPtr address)
{
	uint32_t value;
	::ReadProcessMemory(static_cast<HANDLE>(Handle), static_cast<LPCVOID>(address), &value, sizeof(value), NULL);
	return value;
}

int32_t Win32Process::WriteBytes(System::IntPtr address, array<Byte>^ bytes)
{
	pin_ptr<unsigned char> arr = &bytes[0];
	SIZE_T bytesWritten = 0;
	WriteProcessMemory(static_cast<HANDLE>(Handle), static_cast<LPVOID>(address), arr, bytes->Length, &bytesWritten);
	return bytesWritten;
}

IEnumerable<Win32Thread^>^ Win32Process::Threads::get()
{
	List<Win32Thread^>^ result = gcnew List<Win32Thread^>();
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te)) {
			do {
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
					sizeof(te.th32OwnerProcessID)) {
					result->Add(gcnew Win32Thread(te.th32ThreadID));
				}
				te.dwSize = sizeof(te);
			} while (Thread32Next(h, &te));
		}
		::CloseHandle(h);
	}
	return result;
}

IEnumerable<Win32Module^>^ Win32Process::Modules::get()
{
	List<Win32Module^>^ result = gcnew List<Win32Module^>();
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	//  Take a snapshot of all modules in the specified process. 
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->Id);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return result;
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(hModuleSnap, &me32))
	{
		::CloseHandle(hModuleSnap);
		return result;
	}

	do
	{
		result->Add(gcnew Win32Module(this->Handle, static_cast<IntPtr>(me32.hModule)));
	} while (Module32Next(hModuleSnap, &me32));

	::CloseHandle(hModuleSnap);
	return result;
}

void Win32Process::Suspend()
{
	for each (auto thread in Threads)
		thread->Suspend();
}

void Win32Process::Resume()
{
	for each (auto thread in Threads)
		thread->Resume();
}

#pragma endregion

System::IntPtr Win32Process::GetProcAddress(System::IntPtr module, System::String^ functionName)
{
	const char* cFunctionName = (char*)Marshal::StringToHGlobalAnsi(functionName).ToPointer();
	return static_cast<System::IntPtr>(::GetProcAddress(static_cast<HMODULE>(static_cast<HANDLE>(module)), cFunctionName));
}

IntPtr Win32Process::Load(System::String^ dll)
{
	pin_ptr<const wchar_t> wDll = PtrToStringChars(dll);
	return static_cast<IntPtr>(LoadLibrary(wDll));
}

Win32Thread^ Win32Process::Execute(IntPtr address)
{
	return gcnew Win32Thread(Win32Interop::CreateRemoteThread(this->Handle, address));
}

Win32Thread^ Win32Process::Execute(IntPtr address, IntPtr argument)
{
	return gcnew Win32Thread(Win32Interop::CreateRemoteThread(this->Handle, address, argument));
}

WaitForType Win32Process::WaitForExit()
{
	return static_cast<WaitForType>(::WaitForSingleObject(static_cast<HANDLE>(Handle), INFINITE));
}

IntPtr Win32Process::Alloc(int32_t size, AllocationType allocation, ProtectType protect)
{
	return static_cast<IntPtr>(::VirtualAlloc(NULL, size, static_cast<DWORD>(allocation), static_cast<DWORD>(protect)));
}

bool Win32Process::Free(IntPtr address)
{
	return Win32Interop::VirtualFreeEx(Handle, address, 0, FreeType::MemRelease);
}

void SharpUnmanaged::Win32Process::CloseHandle(System::IntPtr handle)
{
	::CloseHandle(static_cast<HANDLE>(handle));
}

uint32_t SharpUnmanaged::Win32Process::GetModuleSize(System::IntPtr handle, System::IntPtr module)
{
	MODULEINFO mi;
	::GetModuleInformation(static_cast<HANDLE>(handle), static_cast<HMODULE>(static_cast<HANDLE>(module)), &mi, sizeof(mi));
	return mi.SizeOfImage;
}

System::IntPtr SharpUnmanaged::Win32Process::GetModuleEntry(System::IntPtr module)
{
	MODULEINFO mi;
	::GetModuleInformation(static_cast<HANDLE>(Handle), static_cast<HMODULE>(static_cast<HANDLE>(module)), &mi, sizeof(mi));
	return static_cast<IntPtr>(mi.EntryPoint);
}

int32_t Win32Process::WriteString(IntPtr address, String^ value)
{
	const char* buffer = (const char*)(Marshal::StringToHGlobalAnsi(value)).ToPointer();
	SIZE_T bytesWritten = 0;
	::WriteProcessMemory(static_cast<HANDLE>(Handle), static_cast<LPVOID>(address), buffer, strlen(buffer), &bytesWritten);
	return bytesWritten;
}

uint32_t Win32Process::WriteString(System::IntPtr handle, System::IntPtr address, System::String^ value)
{
	const char* buffer = (const char*)(Marshal::StringToHGlobalAnsi(value)).ToPointer();
	SIZE_T bytesWritten = 0;
	::WriteProcessMemory(static_cast<HANDLE>(handle), static_cast<LPVOID>(address), buffer, strlen(buffer), &bytesWritten);
	return bytesWritten;
}

uint32_t Win32Process::CopyMemoryRemote(System::IntPtr fromHandle, System::IntPtr address, const uint32_t length, System::IntPtr toHandle, System::IntPtr remoteAddress)
{
	uint8_t* buffer = new uint8_t[length];
	ReadProcessMemory(static_cast<HANDLE>(fromHandle), static_cast<LPVOID>(address), buffer, length, NULL);

	SIZE_T bytesWritten;
	WriteProcessMemory(static_cast<HANDLE>(toHandle), static_cast<LPVOID>(remoteAddress), buffer, length, &bytesWritten);
	return bytesWritten;
}