/*
 * Created by SharpDevelop.
 * User: hekar
 * Date: 3/9/2011
 * Time: 4:17 PM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.IO;

namespace ExtractLuaBindings
{
	class Program
	{
		public static void Main(string[] args)
		{
            string root = @"C:\Users\hekar\Desktop\code\source_sdk_code\luminousforts-codebase\src\game\server";
            Converter converter = new Converter(new CppToLuaBindings());
            converter.ScanFolders(root);
            converter.Convert(@"C:\users\hekar\desktop\tmp\cpptolua");
		}
	}
}