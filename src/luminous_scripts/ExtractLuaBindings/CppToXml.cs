using System;
using System.IO;
using System.Diagnostics;

namespace ExtractLuaBindings
{
    class CppToXml : ExtractLuaBindings.ICppToX
    {
        public CppToXml()
        {

        }

        public void Start(string infilename, string outfilename)
        {
            string filename = Shared.kCppXmlPath;
            string arguments = String.Join(" ", Shared.kCppXmlDefaultArguments) +
                String.Join(" -I ", Shared.kCppXmlIncludeFolders) +
                " -o \"" + outfilename + "\""  + " \"" + infilename + "\"";

            if (File.Exists(outfilename))
            {
                Console.WriteLine(outfilename + " already exists");
            }
            else
            {
                Console.WriteLine("Executing " + filename);
                Console.WriteLine(arguments);

                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.FileName = filename;
                startInfo.Arguments = arguments;
                startInfo.UseShellExecute = false;
                Process proc = Process.Start(startInfo);
                proc.WaitForExit();

                Console.WriteLine();
            }
        }


        public string GetFilename(string filename)
        {
            filename = Path.GetFileNameWithoutExtension(filename);
            return filename + ".xml";
        }
    }
}
