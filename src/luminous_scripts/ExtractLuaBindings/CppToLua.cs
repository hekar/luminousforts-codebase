using System;
using System.IO;
using System.Diagnostics;

namespace ExtractLuaBindings
{
    class CppToLuaBindings : ICppToX
    {
        public void Start(string infilename, string outfilename)
        {
            string filename = @"C:\Python27\python.exe";
            string arguments = Shared.kCppToLuaPath + " --in " + infilename +
                " --out " + outfilename;

            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = filename;
            startInfo.Arguments = arguments;
            startInfo.UseShellExecute = false;
            Process proc = Process.Start(startInfo);
            proc.WaitForExit();

            Console.WriteLine();
        }

        public string GetFilename(string filename)
        {
            filename = Path.GetFileNameWithoutExtension(filename);
            return "Lua_" + filename + ".cpp";
        }
    }
}
