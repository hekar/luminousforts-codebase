using System;
using System.Collections.Generic;
using System.IO;

namespace ExtractLuaBindings
{
    class Converter
    {
        static string[] ExcludeList = new string[]
        {
            "weapon_", "ai_", "wcedit", "vehicle_", "variant_t", "timedeventmgr", "sdk_player_resource",
            "npc_", "textstatmgr", "stressentities", "pushentity", "playerinfomanager"
        };

        List<string> files;
        ICppToX converter;
        public Converter(ICppToX converter)
        {
            this.converter = converter;
        }

        public void ScanFolders(string root_folder, string ext = "*.h")
        {
            files = new List<string>(Directory.GetFiles(root_folder, ext, SearchOption.AllDirectories));
        }

        public void Convert(string out_folder)
        {
            out_folder = out_folder
                .Replace("\\", Path.DirectorySeparatorChar.ToString())
                .Replace("/", Path.DirectorySeparatorChar.ToString());

            if (!out_folder.EndsWith(Path.DirectorySeparatorChar.ToString()))
            {
                out_folder += Path.DirectorySeparatorChar;
            }

            CppToXml cppxml = new CppToXml();
            int i = 0;
            foreach (var file in files)
            {
                bool skip = false;
                string basename = Path.GetFileNameWithoutExtension(file);
                foreach (var exclude in ExcludeList)
                {
                    if (basename.Contains(exclude))
                    {
                        skip = true;
                        break;
                    }
                }

                if (skip)
                {
                    continue;
                }

                i++;
                string outfilename = out_folder + this.converter.GetFilename(basename);
                this.converter.Start(file, outfilename);
                Console.Clear();
                Console.WriteLine("On file {0}/{1}", i, files.Count);
            }
        }
    }
}
