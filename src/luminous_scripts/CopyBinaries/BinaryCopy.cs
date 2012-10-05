/*
  BinaryCopy.cs -- interface of the CopyBinaries application
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
 * Time: 3:50 PM
 */

using System;
using System.IO;

namespace CopyBinaries
{
	/// <summary>
	/// Description of BinaryCopy.
	/// </summary>
	public class BinaryCopy
	{
		string filepath;
		public BinaryCopy(string filepath)
		{
			this.filepath = filepath;
		}
		
		public void Move(string dest)
		{
			if (File.Exists(dest))
			{
				File.Delete(dest);
			}
			
			File.Move(this.filepath, dest);
		}
		
		public void Copy(string dest)
		{
			if (File.Exists(dest))
			{
				File.Delete(dest);
			}
			
			File.Copy(this.filepath, dest);
		}		
	}
}
