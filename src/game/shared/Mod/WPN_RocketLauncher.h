/* ***** BEGIN LICENSE BLOCK *****
Version: MPL 1.1/LGPL 2.1/GPL 2.0

The contents of this file are subject to the Mozilla Public License Version 
1.1 (the "License"); you may not use this file except in compliance with
...
for the specific language governing rights and limitations under the
License.

The Original Code is for LuminousForts.

The Initial Developer of the Original Code is Hekar Khani.
Portions created by the Hekar Khani are Copyright (C) 2010
Hekar Khani. All Rights Reserved.

Contributor(s):
  Hekar Khani <hekark@gmail.com>

Alternatively, the contents of this file may be used under the terms of
either of the GNU General Public License Version 2 or later (the "GPL"),
...
the terms of any one of the MPL, the GPL or the LGPL.

***** END LICENSE BLOCK ***** */


#ifndef WPN_ROCKETLAUNCHER_H
#define WPN_ROCKETLAUNCHER_H

#ifdef WIN32
#pragma once
#endif // WIN32

#include "weapon_sdkbase.h"

#ifdef CLIENT_DLL
	#include "iviewrender_beams.h"
#endif

#ifndef CLIENT_DLL


#endif // !CLIENT_DLL

class CRocketLauncher : public CWeaponSDKBase
{
	DECLARE_CLASS( CRocketLauncher, CWeaponSDKBase );
public:
	CRocketLauncher();

	void NotifyRocketDied();
};


#endif // WPN_ROCKETLAUNCHER_H