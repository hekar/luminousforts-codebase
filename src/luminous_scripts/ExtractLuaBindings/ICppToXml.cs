using System;
namespace ExtractLuaBindings
{
    interface ICppToX
    {
        string GetFilename(string filename);
        void Start(string infilename, string outfilename);
    }
}
