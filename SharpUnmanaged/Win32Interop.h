#pragma once

#include <cstdint>
#include "windows.h"
#undef CreateProcess
#undef GetModuleHandle
#include <Psapi.h>
#include <comdef.h>

using namespace System;

namespace SharpUnmanaged {
	[FlagsAttribute]
	public enum class FreeType : uint32_t
	{
		MemDecommit = 0x4000,
		MemRelease = 0x8000,
	};

	public enum class WaitForType : uint32_t
	{
		StateSignaled = 0x0,
		Abandoned = 0x80,
		Timeout = 0x102,
		Failed = 0xFFFFFFFF,
	};

	[FlagsAttribute]
	public enum class AllocationType : uint32_t
	{
		MemCommit = 0x00001000,
		MemReserve = 0x00002000,
		MemReset = 0x00080000,
		MemResetUndo = 0x10000000,
	};

	[FlagsAttribute]
	public enum class ProtectType : uint32_t
	{
		NoAccess = 0x01,
		ReadOnly = 0x02,
		ReadWrite = 0x04,
		WriteCopy = 0x08,
	};

	public value struct ModuleInfo
	{
		IntPtr BaseAddress;
		int32_t SizeOfImage;
		IntPtr EntryPoint;
	};

	public ref class Win32Interop
	{
	public:
		static WaitForType WaitForSingleObject(IntPtr handle, int32_t ms);
		static WaitForType WaitForSingleObjectInfinite(IntPtr handle);
		static bool CloseHandle(IntPtr handle);
		static IntPtr Alloc(IntPtr process, IntPtr address, int size, AllocationType allocation, ProtectType protect);
		static IntPtr CreateRemoteThread(IntPtr process, IntPtr entryAddress, IntPtr argumentAddress);
		static IntPtr CreateRemoteThread(IntPtr process, IntPtr entryAddress);
		static bool VirtualFreeEx(IntPtr process, IntPtr regionAddress, int32_t size, FreeType freeType);
		static bool GetThreadTimes(IntPtr handle, [Runtime::InteropServices::Out]DateTime ^%creationTime, [Runtime::InteropServices::Out]DateTime ^%exitTime, [Runtime::InteropServices::Out]DateTime ^%kernelTime, [Runtime::InteropServices::Out]DateTime ^%userTime);
		static int32_t GetProcessId(System::IntPtr handle);
		static String^ GetModuleFileEx(IntPtr process, IntPtr module);
		static String^ GetModuleFileEx(IntPtr process, IntPtr module, int32_t size);
		static ModuleInfo GetModuleInfo(IntPtr process, IntPtr module);
		static String^ GetModuleName(IntPtr process, IntPtr module);
		static IntPtr GetProcAddress(IntPtr module, String^ exportedProcName);
		static array<int32_t>^ GetThreads();
		static array<int32_t>^ GetThreads(int32_t processId);
		static int32_t GetProcessIdFromThreadId(int32_t threadId);
	};
}