#include "stdafx.h"
#include "Win32Module.h"
#include "Win32Interop.h"

using namespace SharpUnmanaged;

Win32Module::Win32Module(IntPtr process, IntPtr handle)
{
	if (process == IntPtr::Zero) throw gcnew ArgumentException("Process must not be zero.");
	this->process = process;
	if (handle == IntPtr::Zero) throw gcnew ArgumentException("Handle must not be zero.");
	this->handle = handle;

	auto info = Win32Interop::GetModuleInfo(process, handle);
	this->entryPoint = info.EntryPoint;
	this->baseAddress = info.BaseAddress;
	this->size = info.SizeOfImage;
	this->name = Win32Interop::GetModuleName(process, handle);
}

Win32Module::~Win32Module()
{
	::CloseHandle(static_cast<HANDLE>(handle));
}

IntPtr Win32Module::Process::get()
{
	return this->process;
}

IntPtr Win32Module::Handle::get()
{
	return this->handle;
}

String^ Win32Module::FileName::get()
{
	return Win32Interop::GetModuleFileEx(process, handle);
}

IntPtr Win32Module::EntryPoint::get()
{
	return this->entryPoint;
}

IntPtr Win32Module::BaseAddress::get()
{
	return this->baseAddress;
}

int32_t Win32Module::Size::get()
{
	return this->size;
}

String^ Win32Module::Name::get()
{
	return this->name;
}

IntPtr Win32Module::GetProcAddress(String^ exportedFuncName)
{
	return Win32Interop::GetProcAddress(this->Handle, exportedFuncName);
}