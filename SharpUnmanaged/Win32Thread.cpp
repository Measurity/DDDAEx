#include "stdafx.h"
#include "Win32Interop.h"
#include "Win32Thread.h"

using namespace SharpUnmanaged;

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
	Win32Thread(Id, ThreadAccess::All);
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
	//if (threadId <= 0) throw gcnew ArgumentException("ThreadId must be bigger than 0.");
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

DateTime Win32Thread::Created::get()
{
	DateTime^ created = gcnew DateTime();
	DateTime^ exitTime = gcnew DateTime();
	DateTime^ kernelTime = gcnew DateTime();
	DateTime^ userTime = gcnew DateTime();
	SharpUnmanaged::Win32Interop::GetThreadTimes(Handle, created, exitTime, kernelTime, userTime);
	return *created;
}

ThreadRegisters Win32Thread::Registers::get()
{
	CONTEXT con;
	::GetThreadContext(static_cast<HANDLE>(Handle), &con);

	// Set struct for managed.
	ThreadRegisters reg;
	reg.Edi = static_cast<IntPtr>((int64_t)con.Edi);
	reg.Esi = static_cast<IntPtr>((int64_t)con.Esi);
	reg.Ebx = static_cast<IntPtr>((int64_t)con.Ebx);
	reg.Edx = static_cast<IntPtr>((int64_t)con.Edx);
	reg.Ecx = static_cast<IntPtr>((int64_t)con.Ecx);
	reg.Eax = static_cast<IntPtr>((int64_t)con.Eax);

	//reg.Ebp = static_cast<IntPtr>((int64_t)con.Ebp);
	reg.Eip = static_cast<IntPtr>((int64_t)con.Eip);
	//reg.SegCs = static_cast<IntPtr>((int64_t)con.SegCs);
	//reg.EFlags = static_cast<IntPtr>((int64_t)con.EFlags);
	//reg.Esp = static_cast<IntPtr>((int64_t)con.Esp);
	//reg.SegSs = static_cast<IntPtr>((int64_t)con.SegSs);

	return reg;
}

Win32Thread::~Win32Thread()
{
	::CloseHandle(static_cast<HANDLE>(Handle));
}

WaitForType Win32Thread::Wait(int32_t ms)
{
	return Win32Interop::WaitForSingleObject(this->Handle, ms);
}

WaitForType Win32Thread::WaitForExit()
{
	return Win32Interop::WaitForSingleObjectInfinite(this->Handle);
}