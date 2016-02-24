#pragma once

#include <cstdint>
#include "windows.h"
#undef CreateProcess
#undef GetModuleHandle
#include <Psapi.h>
#include <comdef.h>

#include "Win32Interop.h"

namespace SharpUnmanaged
{
	public ref class Win32Module
	{
	private:
		IntPtr process;
		IntPtr handle;
		String^ name;
		IntPtr entryPoint;
		IntPtr baseAddress;
		int32_t size;
	public:
		property IntPtr Process { IntPtr get(); }
		property IntPtr Handle { IntPtr get(); }
		property String^ Name { String^ get(); }
		property String^ FileName { String^ get(); }
		property IntPtr EntryPoint { IntPtr get(); }
		property IntPtr BaseAddress { IntPtr get(); }
		property int32_t Size { int32_t get(); }

		Win32Module(IntPtr process, IntPtr handle);
		~Win32Module();

		IntPtr GetProcAddress(String^ exportedFuncName);
	};
}

