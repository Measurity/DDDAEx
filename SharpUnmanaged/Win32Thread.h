#pragma once

#include <cstdint>
#include "windows.h"

using namespace System;

namespace SharpUnmanaged
{
	[FlagsAttribute]
	public enum class ThreadAccess : uint32_t
	{
		All = 0x00100000L,
		Delete = 0x00010000L,
		ReadControl = 0x00020000L,
		Synchronize = 0x00100000L,
		WriteDac = 0x00040000L,
		WriteOwner = 0x00080000L,
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

	public value struct ThreadRegisters
	{
		// Integer registers.
		IntPtr Edi;
		IntPtr Esi;
		IntPtr Ebx;
		IntPtr Edx;
		IntPtr Ecx;
		IntPtr Eax;

		// Control registers.
		IntPtr Ebp;
		IntPtr Eip;
		IntPtr SegCs;
		IntPtr EFlags;
		IntPtr Esp;
		IntPtr SegSs;
	};

	public ref class Win32Thread
	{
	private:
		DateTime created;
	public:
		int32_t Id;
		IntPtr Handle;
		DateTime ExitTime;
		property DateTime Created { DateTime get(); }
		property IntPtr ExitCode { IntPtr get(); };
		property ThreadRegisters Registers { ThreadRegisters get(); }

		Win32Thread(int32_t threadId);
		Win32Thread(int32_t threadId, ThreadAccess access);
		Win32Thread(IntPtr handle);
		~Win32Thread();

		WaitForType Wait(int32_t ms);
		WaitForType WaitForExit();

		void Suspend();
		static void Suspend(int32_t threadId);
		static void Suspend(IntPtr handle);

		void Resume();
		static void Resume(int32_t threadId);
		static void Resume(IntPtr handle);
	};


}