using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Win32;
using SharpUnmanaged;

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
                var key = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry64).OpenSubKey($@"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App {SteamGameId}");
                installPath = (string) key?.GetValue("InstallLocation");
                return installPath;

            }
        }

        public static string ExePath => Path.Combine(InstallPath, "DDDA.exe");

        public static Win32Process LaunchGame(ProcessCreationType creationType = ProcessCreationType.Default)
        {
            if (ExePath == null) return null;
            return Win32Process.Create(ExePath, creationType);
        }
    }
}