/*
  Program.cs -- interface of the CopyBinaries application
  version 1.2.2, October 3rd, 2004

  Copyright (C) 2011 Hekar Khani

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
/*
 * Created by SharpDevelop.
 * User: hekar
 * Date: 3/6/2011
 * Time: 3:48 PM
 */
 
using System;
using System.IO;

namespace CopyBinaries
{
	class Program
	{
		public static int Main(string[] args)
		{
			bool cut_on_move = true;
			foreach (var arg in args)
			{
				if (arg == "--cut")
				{
					cut_on_move = true;
				}
				else if (arg == "--copy")
				{
					cut_on_move = false;
				}
			}
			
			Log.Write("Current Directory " + Directory.GetCurrentDirectory());
		
			const string kBinaryFolder = @"..\..\game_obj\";			
			
			// Binary name (ie. debug_client, release_client, 
			// debug_server, release_server)
			string binary = "";
			
			// Relative path to binary file
			string binary_fullpath = "";
			
			// Binaries' filename
			string binary_dll = "";
			try
			{
				binary = args[0];
				
				binary_dll = binary.ToLower()
						.Replace("debug_", "")
						.Replace("release_", "")
						+ ".dll";
				
				// Capitalize the filename
				binary_dll = char.ToUpper(binary_dll[0]) + binary_dll.Substring(1);
				
				binary_fullpath =  kBinaryFolder + binary + "_final" + 
					Path.DirectorySeparatorChar + 
					binary_dll;
				
				if (!File.Exists(binary_fullpath))
			    {
					throw new FileNotFoundException();
			    }
				
			} 
			catch (IndexOutOfRangeException) 
			{
				Log.Write("Missing Binary path argument");
				return 1;
			}
			catch (FileNotFoundException)
			{
				Log.Write("Binary File does not exist:" + binary_fullpath);
				return 2;
			}
			
			string sourcemod = "";
			// if there is a 2nd command line argument read the sourcemod folder
			// from the command line
			if (args.Length > 1)
			{
				try
				{
					sourcemod = args[1];
					
					if (!Directory.Exists(sourcemod))
				    {
						throw new DirectoryNotFoundException();
				    }
					
				} 
				catch (IndexOutOfRangeException) 
				{
					Log.Write("Missing Sourcemods folder argument");
					return 3;
				}
				catch (DirectoryNotFoundException)
				{
					Log.Write("Cannot write to directory " + sourcemod);
					return 4;
				}
			}
			else
			{
				// Read the config from the users configuration file
				const string kConfigFolder = @"..\..\luminous_dev_config";
				string username = Environment.UserName.ToLower();
				const string suffix = "_sourcemod.txt";
				
				string filepath = kConfigFolder + Path.DirectorySeparatorChar +
					username + suffix;
				
				// Luminous Forts mod directory is on the first line
				StreamReader reader = new StreamReader(filepath);
				sourcemod = reader.ReadLine();
				reader.Close();
			}
			
			if (!sourcemod.EndsWith("/") || !sourcemod.EndsWith("\\"))
			{
				sourcemod += Path.DirectorySeparatorChar;
			}
			
			sourcemod += "bin" + Path.DirectorySeparatorChar;
			
			// Copy the files to their destination
			BinaryCopy copy = new BinaryCopy(binary_fullpath);
			try
			{
				if (cut_on_move)
				{
					copy.Move(sourcemod + binary_dll);
				}
				else
				{
					copy.Copy(sourcemod + binary_dll);
				}
			}
			catch (IOException)
			{
				Log.Write("Failure to move file");
				return 5;
			}
			
			File.Create(kBinaryFolder + binary + ".txt");
			Log.Write("Successfully moved file: " + binary);
			
			return 0;
		}
	}
}