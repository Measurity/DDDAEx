// ManagedWin.h

#pragma once
#include <cstdint>
#include "windows.h"
#undef CreateProcess
#undef GetModuleHandle

using namespace System;
using namespace System::Runtime::InteropServices;

namespace ManagedWin {
	[FlagsAttribute]
	public enum class ProcessAccess : unsigned int
	{
		CreateThread = 0x0002,
		Operation = 0x0008,
		Read = 0x0010,
		Write = 0x0020,
		CreateProcess = 0x0080,
		Information = 0x0400,
		SuspendResume = 0x0800,
		Inject = Operation | Read | Write | CreateThread | Information,
		All = 0x001F0FFF
	};

	[FlagsAttribute]
	public enum class AllocationType : unsigned int
	{
		MemCommit = 0x00001000,
		MemReserve = 0x00002000,
		MemReset = 0x00080000,
		MemResetUndo = 0x10000000,
	};

	[FlagsAttribute]
	public enum class ProtectType : unsigned int
	{
		NoAccess = 0x01,
		ReadOnly = 0x02,
		ReadWrite = 0x04,
		WriteCopy = 0x08
	};

	[FlagsAttribute]
	public enum class FreeType : unsigned int
	{
		MemDecommit = 0x4000,
		MemRelease = 0x8000
	};

	public enum class WaitForType : unsigned int
	{
		StateSignaled = 0x0,
		Abandoned = 0x80,
		Timeout = 0x102,
		Failed = 0xFFFFFFFF,
	};

	public ref class Win32Process
	{
	public:
		// Process management.
		static System::IntPtr OpenProcess(ProcessAccess access, int pid);
		static System::IntPtr GetModuleHandle(System::String^ moduleName);
		static System::IntPtr GetProcAddress(System::IntPtr module, System::String^ functionName);
		static System::IntPtr Execute(System::IntPtr handle, System::IntPtr entryAddress, System::IntPtr argumentAddress);
		static System::IntPtr Alloc(System::IntPtr process, System::IntPtr address, int size, AllocationType allocation, ProtectType protect);
		static System::IntPtr Alloc(System::IntPtr process, System::IntPtr address, unsigned long size, AllocationType allocation, ProtectType protect);
		static System::IntPtr Load(System::String^ dll);
		static WaitForType WaitInfinite(System::IntPtr handle);
		static System::IntPtr GetExitCodeThread(System::IntPtr handle);
		static bool Free(System::IntPtr process, System::IntPtr address);
		static bool Free(System::IntPtr process, System::IntPtr address, unsigned long size, FreeType freeType);
		static void CloseHandle(System::IntPtr handle);

		// Direct memory access.
		static uint32_t WriteString(System::IntPtr handle, System::IntPtr address, System::String^ value);
		static int ReadInt(System::IntPtr handle, System::IntPtr address);
	};
}
