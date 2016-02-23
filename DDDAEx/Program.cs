﻿using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using DDDAEx.Net;
using DDDAEx.Net.Packets;
using SharpDX;
using SharpDX.Direct3D;
using SharpDX.Direct3D9;
using SharpDX.Mathematics.Interop;
using SharpUnmanaged;

namespace DDDAEx
{
    internal static class Program
    {
        private const string LocalHosterPath = @"DDDAExHoster.dll";

        private static void Main()
        {
            Inject();
        }

        private static void Inject()
        {
            try
            {
                // Get process handle.
                using (var proc = DDDAExUtil.LaunchGame())
                {
                    Debug.WriteLine("Waiting for Steam to launch DDDA..");
                    proc.WaitForExit();
                }

                Debug.WriteLine("Attempting to capture DDDA process..");
                // Capture game process.
                Process process;
                do
                {
                    process = Process.GetProcessesByName("DDDA").FirstOrDefault();
                    Task.Delay(10).Wait();
                } while (process == null);
                procHandle = new Win32Process(process.Id, ProcessAccess.All);

                Debug.WriteLine("Suspending DDDA process..");
                // Suspend process.
                process.Threads.Cast<ProcessThread>().ToList().ForEach(t =>
                {
                    var threadH = Win32Process.OpenThread(ThreadAccess.SuspendResume, t.Id);
                    Win32Process.SuspendThread(threadH);
                });
                procMainThread = Win32Process.OpenThread(ThreadAccess.AllAccess,
                    process.Threads.Cast<ProcessThread>().OrderBy(t =>
                    {
                        try
                        {
                            return t.StartTime;
                        }
                        catch
                        {
                            // ignored
                        }
                        return DateTime.Now - TimeSpan.FromDays(1);
                    }).First().Id);

                var threadP = Win32Process.GetThreadInstruction(procMainThread);

                Debug.WriteLine("Reading main thread bytes..");
                // Copy first two bytes of thread entry.
                byte[] threadPStart = Win32Process.ReadBytes(procHandle, threadP, 2);

                Debug.WriteLine("Writing loop in main thread location..");
                // Write infinite loop.
                Win32Process.WriteBytes(procHandle, threadP, new byte[] { 0xEB, 0xFE });

                Debug.WriteLine("Waiting for DLLs to load..");
                // Resume and wait for kernel32 to load. Then pause.
                Win32Process.ResumeThread(procMainThread);
                var targetProc = Process.GetProcessById(Win32Process.GetProcessId(procHandle));
                while (!targetProc.Modules.Cast<ProcessModule>().Any(m => m.ModuleName.IndexOf("kernel32", StringComparison.OrdinalIgnoreCase) >= 0))
                    Thread.Sleep(10);
                Win32Process.SuspendThread(procMainThread);

                Debug.WriteLine("Restoring original thread code..");
                // Remove infinite loop from target process and resume game.
                Win32Process.WriteBytes(procHandle, threadP, threadPStart);
                Win32Process.ResumeThread(procMainThread);

                Debug.WriteLine("Getting LoadLibraryW offset..");
                // Get the LoadLibraryW function pointer from kernel32.dll.
                var loadLibFunc = Win32Process.GetProcAddress(Win32Process.GetModuleHandle("kernel32.dll"), "LoadLibraryA");

                Debug.WriteLine($"Writing CLR hoster dll path into process.. loadLibFunc: 0x{loadLibFunc.ToString("X2")}");
                // Write dll path in process.
                var fullPath = Path.GetFullPath(LocalHosterPath);
                alloc = Win32Process.Alloc(procHandle, IntPtr.Zero, fullPath.Length,
                    AllocationType.MemReserve | AllocationType.MemCommit, ProtectType.ReadWrite);
                Win32Process.WriteString(procHandle, alloc, fullPath);

                Debug.WriteLine("Retrieve ExecuteCLR function offset..");
                // Load library in current process (to retrieve function pointer offset).
                var currentDll = proc.Load(LocalHosterPath);
                var clrFuncOffset = IntPtr.Subtract(Win32Process.GetProcAddress(currentDll, "ExecuteCLR"),
                    currentDll.ToInt32());

                // Resume process (to make sure that LoadLibrary gets called and not dead-locking).
                process.Threads.Cast<ProcessThread>().ToList().ForEach(t => Win32Thread.Resume(t.Id));

                // Load library in target process.
                WaitForType waitResult;
                IntPtr targetLibBase;
                do
                {
                    Debug.WriteLine($"Loading CLR hoster into target.. procHandle:0x{procHandle.ToString("X2")}, loadLibFunc:0x{loadLibFunc.ToString("X2")}, alloc:0x{alloc.ToString("X2")}");
                    var loadLibThread = Win32Interop.CreateRemoteThread(procHandle, loadLibFunc, alloc);

                    Debug.WriteLine("Wait for dll to load in target process..");
                    waitResult = Win32Process.Wait(loadLibThread, 3 * 1000);

                    targetLibBase = Win32Process.GetExitCodeThread(loadLibThread);
                    Debug.WriteLine($"Wait result: {waitResult}");
                } while (waitResult != WaitForType.StateSignaled);

                var remoteExecClrPtr = IntPtr.Add(targetLibBase, clrFuncOffset.ToInt32());

                // Wait for the process to initalize a bit..
                Task.Delay(1000).Wait();

                Win32Interop.CreateRemoteThread(procHandle, remoteExecClrPtr);
            }
            finally
            {
                // Release resources.
                //Win32Process.Free(procHandle, alloc);
                //Win32Process.CloseHandle(procThreadHandle);
                //Win32Process.CloseHandle(procHandle);
            }
        }

        private static int Init(string argument)
        {
            MessageBox.Show("C# injected :D");

            //PresentParameters parms = new PresentParameters();
            //parms.Windowed = true;
            //parms.SwapEffect = SwapEffect.Discard;
            //parms.BackBufferFormat = Format.A8R8G8B8;
            //Device device = new Device(new Direct3D(), 0, DeviceType.Hardware, Process.GetCurrentProcess().MainWindowHandle, CreateFlags.HardwareVertexProcessing, parms);

            //Task.Factory.StartNew(() =>
            //{
            //    while (true)
            //    {
            //        device.ShowCursor = true;
            //        device.SetCursorPosition(0, 0, true);
            //        Task.Delay(1000).Wait();
            //    }
            //}, TaskCreationOptions.LongRunning);
            return 0;
        }
    }
}