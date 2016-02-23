#pragma once

#include <cstdint>
#include "windows.h"

using namespace System;

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

public ref class Win32Thread
{
public:
	int32_t Id;
	IntPtr Handle;
	DateTime^ Created;
	DateTime^ ExitTime;
	property IntPtr ExitCode { IntPtr get(); };

	Win32Thread(int32_t threadId);
	Win32Thread(int32_t threadId, ThreadAccess access);
	Win32Thread(IntPtr handle);
	~Win32Thread();

	void Suspend();
	static void Suspend(int32_t threadId);
	static void Suspend(IntPtr handle);

	void Resume();
	static void Resume(int32_t threadId);
	static void Resume(IntPtr handle);
};

