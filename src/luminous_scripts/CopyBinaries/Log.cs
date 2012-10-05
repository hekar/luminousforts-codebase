/*
 * Created by SharpDevelop.
 * User: hekar
 * Date: 4/10/2011
 * Time: 2:46 PM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.IO;

namespace CopyBinaries
{
	/// <summary>
	/// Description of Log.
	/// </summary>
	public class Log
	{
		private static bool useFile = false;
		private const string kDefaultLogFile = "build_log.txt";
		public static void Write(string text)
		{
			if (useFile)
			{
				StreamWriter writer = new StreamWriter(kDefaultLogFile);
				writer.WriteLine(text);
				writer.Close();
			}
			else
			{
				Console.WriteLine(text);
			}
		}
	}
}
