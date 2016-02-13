using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using ManagedWin;
using Microsoft.Win32;

namespace DDDAEx
{
    public static class DDDAExUtil
    {
        public static int SteamGameId => 367500;

        private static string installPath;
        public static string InstallPath
        {
            get
            {
                if (installPath != null) return installPath;
                var key = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry32).OpenSubKey(@"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App " + SteamGameId);
                installPath = (string) key?.GetValue("InstallLocation");
                return installPath;
            }
        }

        public static string ExePath => Path.Combine(InstallPath, "DDDA.exe");

        public static Tuple<IntPtr, IntPtr> LaunchGame(ProcessCreationType creationType = ProcessCreationType.Default)
        {
            if (ExePath == null) return null;
            return Win32Process.CreateProcess(ExePath, creationType);
        }
    }
}