using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using ManagedWin;

namespace DDDAEx
{
    static class Program
    {
        const string LocalHosterPath = @"DDDAExHoster.dll";

        static void Main()
        {
            IntPtr procHandle = IntPtr.Zero;
            IntPtr procThreadHandle = IntPtr.Zero;
            IntPtr alloc = IntPtr.Zero;
            try
            {
                // Get process handle.
                var handles = DDDAExUtil.LaunchGame(ProcessCreationType.CreateSuspended);
                procHandle = handles.Item1;
                procThreadHandle = handles.Item2;

                // Copy LoadLibraryA function to target process.
                IntPtr loadLibFunc = Win32Process.GetProcAddress(Win32Process.GetModuleHandle("kernel32.dll"),
                    "LoadLibraryA");
                IntPtr allocLoadLib = Win32Process.Alloc(procHandle, IntPtr.Zero, 200,
                    AllocationType.MemReserve | AllocationType.MemCommit, ProtectType.ReadWrite);
                Win32Process.CopyMemoryRemote(Process.GetCurrentProcess().Handle, loadLibFunc, 200, procHandle,
                    allocLoadLib);

                // Write dll path in process.
                var fullPath = Path.GetFullPath(LocalHosterPath);
                alloc = Win32Process.Alloc(procHandle, IntPtr.Zero, fullPath.Length,
                    AllocationType.MemReserve | AllocationType.MemCommit, ProtectType.ReadWrite);
                Win32Process.WriteString(procHandle, alloc, fullPath);

                // Load library in target process.
                IntPtr loadLibThread = Win32Process.Execute(procHandle, loadLibFunc, alloc);

                // Load library in current process (to retrieve function pointer offset).
                var currentDll = Win32Process.Load(LocalHosterPath);
                var clrFuncOffset = IntPtr.Subtract(Win32Process.GetProcAddress(currentDll, "ExecuteCLR"),
                    currentDll.ToInt32());

                // Wait for library to be loaded in target process.
                Win32Process.WaitInfinite(loadLibThread);
                IntPtr targetLibBase = Win32Process.GetExitCodeThread(loadLibThread);

                // Resume game.
                Win32Process.ResumeThread(procThreadHandle);

                var remoteExecClrPtr = IntPtr.Add(targetLibBase, clrFuncOffset.ToInt32());
                Win32Process.Execute(procHandle, remoteExecClrPtr, IntPtr.Zero);
            }
            finally
            {
                // Release resources.
                Win32Process.Free(procHandle, alloc);
                Win32Process.CloseHandle(procThreadHandle);
                Win32Process.CloseHandle(procHandle);
            }
        }

        static int Init(string argument)
        {
            MessageBox.Show(Process.GetCurrentProcess().MainWindowTitle);
            return 0;
        }
    }
}
