// ManagedWin.h

#pragma once
#include <cstdint>
#include "windows.h"
#undef CreateProcess
#undef GetModuleHandle
#include <Psapi.h>
#include <comdef.h>

#include "Win32Interop.h"
#include "Win32Thread.h"
#include "Win32Module.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace SharpUnmanaged {
	[FlagsAttribute]
	public enum class ProcessAccess : uint32_t
	{
		CreateThread = 0x0002,
		Operation = 0x0008,
		Read = 0x0010,
		Write = 0x0020,
		CreateProcess = 0x0080,
		Information = 0x0400,
		SuspendResume = 0x0800,
		Inject = Operation | Read | Write | CreateThread | Information,
		All = 0x001F0FFF,
	};

	[FlagsAttribute]
	public enum class ProcessCreationType : uint32_t
	{
		Default = 0,
		DebugProcess = 0x00000001,
		DebugOnlyThisProcess = 0x00000002,
		CreateSuspended = 0x00000004,
		DetachedProcess = 0x00000008,
		CreateNewConsole = 0x00000010,
		CreateNewProcessGroup = 0x00000200,
		CreateUnicodeEnvironment = 0x00000400,
		InheritParentAffinity = 0x00010000,
		ExtendedStartupInfoPresent = 0x00080000,
		CreateNoWindow = 0x08000000,
		CreateProtectedProcess = 0x00040000,
	};

	public ref class Win32Process
	{
	private:
		// Static helpers
		static void CloseHandle(System::IntPtr handle);
		static uint32_t GetModuleSize(System::IntPtr handle, System::IntPtr module);

	public:
		int32_t Id;
		IntPtr Handle;
		property IEnumerable<Win32Thread^>^ Threads { IEnumerable<Win32Thread^>^ get(); }
		property IEnumerable<Win32Module^>^ Modules { IEnumerable<Win32Module^>^ get(); }
		
#pragma region Constructors

		Win32Process(int32_t pid);
		Win32Process(int32_t pid, ProcessAccess access);
		Win32Process(IntPtr handle);
		Win32Process(IntPtr handle, ProcessAccess access);
		~Win32Process();

		static Win32Process^ Current();
		static Win32Process^ Create(String^ fileName);
		static Win32Process^ Create(String^ fileName, ProcessCreationType creationType);

#pragma endregion

		IntPtr Load(String^ dll);
		Win32Thread^ Execute(IntPtr address);
		Win32Thread^ Execute(IntPtr address, IntPtr argument);
		WaitForType Win32Process::WaitForExit();
		IntPtr GetModuleHandle(String^ moduleName);
		IntPtr GetModuleEntry(IntPtr module);
		IntPtr Alloc(int32_t size, AllocationType allocation, ProtectType protect);
		bool Free(IntPtr regionAddress);

		void Suspend();
		void Resume();

#pragma region Memory access

		array<Byte>^ ReadBytes(IntPtr address, uint32_t size);
		int32_t WriteBytes(IntPtr address, array<Byte>^ bytes);
		int32_t WriteString(IntPtr address, String^ value);
		int32_t ReadInt(IntPtr address);

#pragma endregion

		static IntPtr GetProcAddress(System::IntPtr module, System::String^ functionName);
		static uint32_t WriteString(System::IntPtr handle, System::IntPtr address, System::String^ value);
		static uint32_t CopyMemoryRemote(System::IntPtr fromHandle, System::IntPtr address, uint32_t length, System::IntPtr toHandle, System::IntPtr remoteAddress);
	};
}
