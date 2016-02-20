// ManagedWin.h

#pragma once
#include <cstdint>
#include "windows.h"
#undef CreateProcess
#undef GetModuleHandle
#include <Psapi.h>
#include <comdef.h>

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
		All = 0x001F0FFF,
	};

	[FlagsAttribute]
	public enum class ThreadAccess : unsigned int
	{
		Delete = 0x00010000L,
		ReadControl = 0x00020000L,
		Synchronize = 0x00100000L,
		WriteDac = 0x00040000L,
		WriteOwner = 0x00080000L,
		AllAccess = 0x00100000L,
		GetContext = 0x0008,
		SetContext = 0x0010,
		Impersonate = 0x0100,
		QueryInformation = 0x0100,
		QueryLimitedInformation = 0x0800,
		SetInformation = 0x0020,
		SetLimitedInformation = 0x0400,
		SetThreadToken = 0x0080,
		SuspendResume = 0x0002,
		Terminate = 0x0001,
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
		WriteCopy = 0x08,
	};

	[FlagsAttribute]
	public enum class FreeType : unsigned int
	{
		MemDecommit = 0x4000,
		MemRelease = 0x8000,
	};

	public enum class WaitForType : unsigned int
	{
		StateSignaled = 0x0,
		Abandoned = 0x80,
		Timeout = 0x102,
		Failed = 0xFFFFFFFF,
	};

	[FlagsAttribute]
	public enum class ProcessCreationType : unsigned int
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
	public:
		// Process management.
		static System::IntPtr OpenProcess(ProcessAccess access, int pid);
		static System::IntPtr OpenThread(ThreadAccess access, int tid);
		static System::Tuple<IntPtr, IntPtr>^ CreateProcess(System::String^ file);
		static System::Tuple<IntPtr, IntPtr>^ CreateProcess(System::String^ file, ProcessCreationType type);
		static void SuspendThread(System::IntPtr thread);
		static void ResumeThread(System::IntPtr thread);
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
		static uint32_t GetModuleSize(System::IntPtr handle, System::IntPtr module);
		static System::IntPtr GetModuleEntry(System::IntPtr handle, System::IntPtr module);
		static System::IntPtr GetProcessModuleHandle(System::IntPtr process, System::String^ module);
		static int32_t GetProcessId(System::IntPtr handle);
		static System::IntPtr GetThreadInstruction(System::IntPtr handle);

		// Direct memory access.
		static uint32_t WriteString(System::IntPtr handle, System::IntPtr address, System::String^ value);
		static uint32_t CopyMemoryRemote(System::IntPtr fromHandle, System::IntPtr address, uint32_t length, System::IntPtr toHandle, System::IntPtr remoteAddress);
		static uint32_t ReadInt(System::IntPtr handle, System::IntPtr address);
		static array<Byte>^ ReadBytes(System::IntPtr process, System::IntPtr base, int32_t length);
		static void WriteBytes(System::IntPtr process, System::IntPtr base, array<Byte>^ bytes);
	};
}
