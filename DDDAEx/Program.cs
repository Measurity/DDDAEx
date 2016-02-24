using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
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
            // Get process handle.
            using (var steamProc = DDDAExUtil.LaunchGame())
            {
                Debug.WriteLine("Waiting for Steam to launch DDDA..");
                steamProc.WaitForExit();
            }

            Debug.WriteLine("Attempting to capture DDDA process..");
            // Capture game process.
            Process process;
            do
            {
                process = Process.GetProcessesByName("DDDA").FirstOrDefault();
                Task.Delay(10).Wait();
            } while (process == null);
            using (var proc = new Win32Process(process.Id, ProcessAccess.All))
            {
                Debug.WriteLine("Suspending DDDA process..");
                proc.Suspend();
                var procMainThread = proc.Threads.OrderBy(t => t.Created).First();
                var threadEip = procMainThread.Registers.Eip;

                Debug.WriteLine("Reading main thread bytes..");
                // Copy first two bytes of thread entry.
                var threadPStart = proc.ReadBytes(threadEip, 2);

                Debug.WriteLine("Writing loop in main thread location..");
                // Write infinite loop.
                proc.WriteBytes(threadEip, new byte[] { 0xEB, 0xFE });

                Debug.WriteLine("Waiting for DLLs to load..");
                // Resume and wait for kernel32 to load. Then pause.
                procMainThread.Resume();
                while (!proc.Modules.Any(m => m.Name.IndexOf("kernel32", StringComparison.OrdinalIgnoreCase) >= 0))
                    Thread.Sleep(10);
                procMainThread.Suspend();

                Debug.WriteLine("Restoring original thread code..");
                // Remove infinite loop from target process and resume game.
                proc.WriteBytes(threadEip, threadPStart);
                procMainThread.Resume();

                Debug.WriteLine("Getting LoadLibraryW offset..");
                // Get the LoadLibraryW function pointer from kernel32.dll.
                var loadLibFunc =
                    proc.Modules.First(m => m.Name.IndexOf("kernel32.dll", StringComparison.OrdinalIgnoreCase) >= 0)
                        .GetProcAddress("LoadLibraryA");

                Debug.WriteLine(
                    $"Writing CLR hoster dll path into process.. loadLibFunc: 0x{loadLibFunc.ToString("X2")}");
                // Write dll path in process.
                var fullPath = Path.GetFullPath(LocalHosterPath);
                var alloc = proc.Alloc(fullPath.Length, AllocationType.MemReserve | AllocationType.MemCommit,
                    ProtectType.ReadWrite);
                proc.WriteString(alloc, fullPath);

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
                    Debug.WriteLine(
                        $"Loading CLR hoster into target.. procHandle:0x{proc.Handle.ToString("X2")}, loadLibFunc:0x{loadLibFunc.ToString("X2")}, alloc:0x{alloc.ToString("X2")}");
                    using (var loadLibThread = proc.Execute(loadLibFunc, alloc))
                    {
                        Debug.WriteLine("Wait for dll to load in target process..");
                        waitResult = loadLibThread.Wait(3 * 1000);

                        targetLibBase = loadLibThread.ExitCode;
                        Debug.WriteLine($"Wait result: {waitResult}");
                    }
                } while (waitResult != WaitForType.StateSignaled);

                var remoteExecClrPtr = IntPtr.Add(targetLibBase, clrFuncOffset.ToInt32());

                // Wait for the process to initalize a bit..
                Task.Delay(1000).Wait();

                proc.Execute(remoteExecClrPtr);
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