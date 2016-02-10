using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows.Forms;
using ManagedWin;

namespace DDDAEx
{
    static class Program
    {
        const string DllPath = @"DDDAExHoster.dll";

        static void Main()
        {
            IntPtr handle = IntPtr.Zero;
            IntPtr alloc = IntPtr.Zero;
            try
            {
                // Write dll path in process.
                handle = Win32Process.OpenProcess(ProcessAccess.Inject,
                    Process.GetProcessesByName("sublime_text")[0].Id);
                alloc = Win32Process.Alloc(handle, IntPtr.Zero, DllPath.Length,
                    AllocationType.MemReserve | AllocationType.MemCommit, ProtectType.ReadWrite);
                Win32Process.WriteString(handle, alloc, Path.GetFullPath(DllPath));

                // Load library in target process.
                IntPtr loadLibFunc = Win32Process.GetProcAddress(Win32Process.GetModuleHandle("kernel32.dll"),
                    "LoadLibraryA");
                IntPtr loadLibThread = Win32Process.Execute(handle, loadLibFunc, alloc);

                // Load library in current process (to retrieve function pointer offset).
                var currentDll = Win32Process.Load(DllPath);
                var clrFuncOffset = IntPtr.Subtract(Win32Process.GetProcAddress(currentDll, "ExecuteCLR"),
                    currentDll.ToInt32());

                // Wait for library to be loaded in target process.
                Win32Process.WaitInfinite(loadLibThread);
                IntPtr targetLibBase = Win32Process.GetExitCodeThread(loadLibThread);

                var remoteExecClrPtr = IntPtr.Add(targetLibBase, clrFuncOffset.ToInt32());
                Win32Process.Execute(handle, remoteExecClrPtr, IntPtr.Zero);
            }
            finally
            {
                // Release resources.
                Win32Process.Free(handle, alloc);
                Win32Process.CloseHandle(handle);
            }
        }

        static int Init(string argument)
        {
            MessageBox.Show(Process.GetCurrentProcess().MainWindowTitle);
            return 0;
        }
    }
}
