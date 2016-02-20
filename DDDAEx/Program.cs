using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows.Forms;
using DDDAEx.Net;
using DDDAEx.Net.Packets;
using ManagedWin;

namespace DDDAEx
{
    internal static class Program
    {
        private const string LocalHosterPath = @"DDDAExHoster.dll";

        private static void Main()
        {
            var peer = Peer.Create();
            var peer2 = Peer.Create(0);

            peer2.Connect("localhost", 8080);
            peer2.TransmitAll(new ChatPacket("Hallo wereld ;D"));

            Thread.Sleep(60000);

            //Inject();
        }

        private static void Inject()
        {
            var procHandle = IntPtr.Zero;
            var procThreadHandle = IntPtr.Zero;
            var alloc = IntPtr.Zero;
            try
            {
                // Get process handle.
                var handles = DDDAExUtil.LaunchGame(ProcessCreationType.CreateSuspended);
                procHandle = handles.Item1;
                procThreadHandle = handles.Item2;

                var threadP = Win32Process.GetThreadInstruction(procThreadHandle);
                
                // Copy first two bytes of thread entry.
                byte[] threadPStart = Win32Process.ReadBytes(procHandle, threadP, 2);

                // Write infinite loop.
                Win32Process.WriteBytes(procHandle, threadP, new byte[] { 0xEB, 0xFE });

                // Resume and wait for kernel32 to load. Then pause.
                Win32Process.ResumeThread(procThreadHandle);
                var targetProc = Process.GetProcessById(Win32Process.GetProcessId(procHandle));
                while (!targetProc.Modules.Cast<ProcessModule>().Any(m => m.ModuleName.IndexOf("kernel32", StringComparison.OrdinalIgnoreCase) >= 0))
                    Thread.Sleep(10);
                Win32Process.SuspendThread(procThreadHandle);

                // Remove infinite loop from target process and resume game.
                Win32Process.WriteBytes(procHandle, threadP, threadPStart);
                Win32Process.ResumeThread(procThreadHandle);

                // Get the LoadLibraryW function pointer from kernel32.dll.
                var loadLibFunc = Win32Process.GetProcAddress(Win32Process.GetModuleHandle("kernel32.dll"), "LoadLibraryA");

                // Write dll path in process.
                var fullPath = Path.GetFullPath(LocalHosterPath);
                alloc = Win32Process.Alloc(procHandle, IntPtr.Zero, fullPath.Length,
                    AllocationType.MemReserve | AllocationType.MemCommit, ProtectType.ReadWrite);
                Win32Process.WriteString(procHandle, alloc, fullPath);

                // Load library in target process.
                var loadLibThread = Win32Process.Execute(procHandle, loadLibFunc, alloc);

                // Load library in current process (to retrieve function pointer offset).
                var currentDll = Win32Process.Load(LocalHosterPath);
                var clrFuncOffset = IntPtr.Subtract(Win32Process.GetProcAddress(currentDll, "ExecuteCLR"),
                    currentDll.ToInt32());

                // Wait for library to be loaded in target process.
                Win32Process.WaitInfinite(loadLibThread);
                var targetLibBase = Win32Process.GetExitCodeThread(loadLibThread);

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

        private static int Init(string argument)
        {
            MessageBox.Show(Process.GetCurrentProcess().MainWindowTitle);
            return 0;
        }
    }
}