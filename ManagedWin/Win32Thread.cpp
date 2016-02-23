#include "stdafx.h"
#include "Win32Thread.h"

Win32Thread::Win32Thread(int32_t threadId)
{
	Win32Thread(threadId, ThreadAccess::All);
}

Win32Thread::Win32Thread(int32_t threadId, ThreadAccess access)
{
	if (threadId <= 0) throw gcnew ArgumentException("ThreadId must not be less or equal to 0.");
	Id = threadId;
	Handle = static_cast<IntPtr>(::OpenThread(static_cast<DWORD>(access), FALSE, threadId));
}

Win32Thread::Win32Thread(IntPtr handle)
{
	if (handle == IntPtr::Zero) throw gcnew ArgumentException("Handle must not be 0.");
	Id = ::GetThreadId(static_cast<HANDLE>(handle));
	if (Id <= 0) throw gcnew ArgumentException("ThreadId must not be less or equal to 0.");
	Handle = handle;
}

void Win32Thread::Suspend()
{
	::SuspendThread(static_cast<HANDLE>(Handle));
}

void Win32Thread::Suspend(int32_t threadId)
{
	if (threadId <= 0) throw gcnew ArgumentException("ThreadId must be bigger than 0.");
	HANDLE handle = ::OpenThread(static_cast<DWORD>(ThreadAccess::SuspendResume), FALSE, threadId);
	::SuspendThread(handle);
	::CloseHandle(handle);
}

void Win32Thread::Suspend(IntPtr handle)
{
	if (handle == IntPtr::Zero) throw gcnew ArgumentException("Handle must not be 0.");
	HANDLE nHandle = static_cast<HANDLE>(handle);
	::SuspendThread(nHandle);
	::CloseHandle(nHandle);
}

void Win32Thread::Resume()
{
	::ResumeThread(static_cast<HANDLE>(Handle));
}

void Win32Thread::Resume(int32_t threadId)
{
	if (threadId <= 0) throw gcnew ArgumentException("ThreadId must be bigger than 0.");
	HANDLE handle = ::OpenThread(static_cast<DWORD>(ThreadAccess::SuspendResume), FALSE, threadId);
	::ResumeThread(handle);
	::CloseHandle(handle);
}

void Win32Thread::Resume(IntPtr handle)
{
	if (handle == IntPtr::Zero) throw gcnew ArgumentException("Handle must not be 0.");
	HANDLE nHandle = static_cast<HANDLE>(handle);
	::ResumeThread(nHandle);
	::CloseHandle(nHandle);
}

IntPtr Win32Thread::ExitCode::get()
{
	DWORD exit;
	::GetExitCodeThread(static_cast<HANDLE>(Handle), &exit);
	return static_cast<System::IntPtr>((int)exit);
}

Win32Thread::~Win32Thread()
{
	::CloseHandle(static_cast<HANDLE>(Handle));
}
