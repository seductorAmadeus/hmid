using System;
using System.Diagnostics;

namespace lab_5
{
    internal class MyProcess
    {
        public Process[] ProcessList;

        public void BindToRunningProcesses()
        {
            ProcessList = Process.GetProcesses();
           
        }

        public double GetNumberOfMemoryPages(Process process) => process.PrivateMemorySize64 / Environment.SystemPageSize;

    }
}
