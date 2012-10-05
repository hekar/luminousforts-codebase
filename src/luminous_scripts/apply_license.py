"""
***** BEGIN LICENSE BLOCK *****
Version: MPL 1.1/LGPL 2.1/GPL 2.0

The contents of this file are subject to the Mozilla Public License Version 
1.1 (the "License"); you may not use this file except in compliance with
...
for the specific language governing rights and limitations under the
License.

The Original Code is for Luminous Forts.

The Initial Developer of the Original Code is Hekar Khani.
Portions created by the Initial Developer are Copyright (C) 2007
the Initial Developer. All Rights Reserved.

Contributor(s):
  Hekar Khani <hekark@gmail.com>

Alternatively, the contents of this file may be used under the terms of
either of the GNU General Public License Version 2 or later (the "GPL"),
...
the terms of any one of the MPL, the GPL or the LGPL.

***** END LICENSE BLOCK *****
"""

# This script really only works well if there's one author

import os
import sys

folders = ['src/game/client/mod', 'src/game/shared/mod', 'src/game/server/mod']
extensions = ['.cpp', '.h']

license = """/* ***** BEGIN LICENSE BLOCK *****
Version: MPL 1.1/LGPL 2.1/GPL 2.0

The contents of this file are subject to the Mozilla Public License Version 
1.1 (the "License"); you may not use this file except in compliance with
...
for the specific language governing rights and limitations under the
License.

The Original Code is for Luminous Forts.

The Initial Developer of the Original Code is Hekar Khani.
Portions created by the Hekar Khani are Copyright (C) 2010
Hekar Khani. All Rights Reserved.

Contributor(s):
  Hekar Khani <hekark@gmail.com>

Alternatively, the contents of this file may be used under the terms of
either of the GNU General Public License Version 2 or later (the "GPL"),
...
the terms of any one of the MPL, the GPL or the LGPL.

***** END LICENSE BLOCK ***** */"""

def apply_license(folder):
	global license, extensions
	files = os.listdir(folder)
	sys.stdout.write("Applying License to:\n")
	for file in files:
		fullname = folder + os.path.sep + file
		sys.stdout.write(fullname + "\n")
		for ext in extensions:
			if file.endswith(ext):
				fd = open(fullname, 'r+')
				# read in everything
				contents = fd.read()
				# seek to beginning
				fd.seek(0)
				
				# write out our new file with the added license
				fd.write(license + '\n\n\n' + contents)
				
				fd.close()
	
if __name__ == '__main__':
	for folder in folders:
		cwd = os.getcwd() + os.path.sep
		apply_folder = cwd + folder.replace('/', os.path.sep)
		apply_license(apply_folder)