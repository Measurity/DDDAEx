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

System::Tuple<IntPtr, IntPtr>^ ManagedWin::Win32Process::CreateProcess(System::String^ file, ProcessCreationType type)
{
	pin_ptr<const wchar_t> wFile = PtrToStringChars(file);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	::ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	::ZeroMemory(&pi, sizeof(pi));
	CreateProcessW(wFile, NULL, NULL, NULL, FALSE, static_cast<DWORD>(type), NULL, NULL, &si, &pi);

	return gcnew System::Tuple<IntPtr, IntPtr>(static_cast<System::IntPtr>(pi.hProcess), static_cast<System::IntPtr>(pi.hThread));
}

System::Tuple<IntPtr, IntPtr>^ ManagedWin::Win32Process::CreateProcess(System::String^ file)
{
	return ManagedWin::Win32Process::CreateProcess(file, ProcessCreationType::Default);
}

void ManagedWin::Win32Process::SuspendThread(System::IntPtr thread)
{
	::SuspendThread(static_cast<HANDLE>(thread));
}

void ManagedWin::Win32Process::ResumeThread(System::IntPtr thread)
{
	::ResumeThread(static_cast<HANDLE>(thread));
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

void ManagedWin::Win32Process::CloseHandle(System::IntPtr handle)
{
	::CloseHandle(static_cast<HANDLE>(handle));
}

int32_t ManagedWin::Win32Process::GetProcessId(System::IntPtr handle)
{
	return ::GetProcessId(static_cast<HANDLE>(handle));
}

System::IntPtr ManagedWin::Win32Process::GetThreadInstruction(System::IntPtr handle)
{
	CONTEXT con;
	::GetThreadContext(static_cast<HANDLE>(handle), &con);
	return static_cast<System::IntPtr>((long long)con.Eip);
}

uint32_t ManagedWin::Win32Process::GetModuleSize(System::IntPtr handle, System::IntPtr module)
{
	MODULEINFO mi;
	::GetModuleInformation(static_cast<HANDLE>(handle), static_cast<HMODULE>(static_cast<HANDLE>(module)), &mi, sizeof(mi));
	return mi.SizeOfImage;
}

System::IntPtr ManagedWin::Win32Process::GetModuleEntry(System::IntPtr handle, System::IntPtr module)
{
	MODULEINFO mi;
	::GetModuleInformation(static_cast<HANDLE>(handle), static_cast<HMODULE>(static_cast<HANDLE>(module)), &mi, sizeof(mi));
	return static_cast<System::IntPtr>(mi.EntryPoint);
}

System::IntPtr ManagedWin::Win32Process::GetProcessModuleHandle(System::IntPtr process, System::String^ module)
{
	throw gcnew NotImplementedException();
	const char* buffer = (const char*)(Marshal::StringToHGlobalAnsi(module)).ToPointer();
	HMODULE hMods[1024];
	HANDLE hProcess = static_cast<HANDLE>(process);
	DWORD cbNeeded;

	if (::EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (uint16_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (::GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				// TODO: Fix compare.. (aka. proper cast to right types)
				_bstr_t comp(szModName);
				if (strcmp(comp, buffer) == 0)
				{
					return static_cast<System::IntPtr>(hMods[i]);
				}
			}
		}
	}

	return IntPtr::Zero;
}

uint32_t ManagedWin::Win32Process::WriteString(System::IntPtr handle, System::IntPtr address, System::String^ value)
{
	const char* buffer = (const char*)(Marshal::StringToHGlobalAnsi(value)).ToPointer();
	SIZE_T bytesWritten = 0;
	::WriteProcessMemory(static_cast<HANDLE>(handle), static_cast<LPVOID>(address), buffer, strlen(buffer), &bytesWritten);
	return bytesWritten;
}

uint32_t ManagedWin::Win32Process::CopyMemoryRemote(System::IntPtr fromHandle, System::IntPtr address, const uint32_t length, System::IntPtr toHandle, System::IntPtr remoteAddress)
{
	uint8_t* buffer = new uint8_t[length];
	ReadProcessMemory(static_cast<HANDLE>(fromHandle), static_cast<LPVOID>(address), buffer, length, NULL);

	SIZE_T bytesWritten;
	WriteProcessMemory(static_cast<HANDLE>(toHandle), static_cast<LPVOID>(remoteAddress), buffer, length, &bytesWritten);
	return bytesWritten;
}

array<Byte>^ ManagedWin::Win32Process::ReadBytes(System::IntPtr process, System::IntPtr base, int32_t length)
{
	uint8_t* buffer = new uint8_t[length];
	ReadProcessMemory(static_cast<HANDLE>(process), static_cast<LPVOID>(base), buffer, length, NULL);

	array<Byte>^ result = gcnew array<Byte>(length);
	pin_ptr<Byte> result_start = &result[0];
	memcpy(result_start, buffer, length);

	return result;
}

void ManagedWin::Win32Process::WriteBytes(System::IntPtr process, System::IntPtr base, array<Byte>^ bytes)
{
	pin_ptr<unsigned char> arr = &bytes[0];
	WriteProcessMemory(static_cast<HANDLE>(process), static_cast<LPVOID>(base), arr, bytes->Length, NULL);

}

uint32_t ManagedWin::Win32Process::ReadInt(System::IntPtr handle, System::IntPtr address)
{
	uint32_t value;
	::ReadProcessMemory(static_cast<HANDLE>(handle), static_cast<LPCVOID>(address), &value, sizeof(value), NULL);
	return value;
}