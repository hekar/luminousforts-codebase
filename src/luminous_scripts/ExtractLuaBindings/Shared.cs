/*
 * Created by SharpDevelop.
 * User: hekar
 * Date: 3/9/2011
 * Time: 4:35 PM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Diagnostics;

namespace ExtractLuaBindings
{
	/// <summary>
	/// Description of Shared.
	/// </summary>
	public class Shared
	{
        public const string kCppToLuaPath = @"C:\Users\hekar\Desktop\cpptolua\cpptolua.py";
        public const string kCppToLuaOut = @"C:\Users\hekar\Desktop\tmp\cpptolua";

        public const string kCppXmlPath = @"..\..\..\..\luminous_third_libs\cpptoxml\cpptoxml.exe";
        public const string kQtIncludeDir = @"C:\Qt\2010.05\qt\include";
        public const string kQtConfigFile = @"..\..\..\..\luminous_third_libs\cpptoxml\pp-qt-configuration-win";
        public static string[] kCppXmlDefaultArguments = new string[] 
        { 
            "-Q", kQtIncludeDir,
            "-qt", kQtConfigFile,
            "-R", "-d"
        };

        public static string[] kCppXmlIncludeFolders = new string[]
        {
            @"..\..\..\..\game\server",
            @"..\..\..\..\game\server\hl2",
            @"..\..\..\..\game\server\hl2mp",
            @"..\..\..\..\game\server\Mod",
            @"..\..\..\..\game\server\sdk",
            @"..\..\..\..\game\shared",
            @"..\..\..\..\game\shared\hl2",
            @"..\..\..\..\game\shared\hl2mp",
            @"..\..\..\..\game\shared\Mod",
            @"..\..\..\..\game\shared\Multiplayer",
            @"..\..\..\..\game\shared\sdk",
            @"..\..\..\..\public",
            @"..\..\..\..\common",
        };

	}
}
