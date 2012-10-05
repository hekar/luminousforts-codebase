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

#ifndef WPN_MEDIKIT_H
#define WPN_MEDIKIT_H

#ifdef WIN32
#pragma once
#endif // WIN32

#define MELEE_HULL_DIM		16

#ifdef CLIENT_DLL
	#define CWeaponMedikit C_WeaponMedikit
#endif // CLIENT_DLL

class CWeaponMedikit : public CWeaponSDKMelee
{
public:
	DECLARE_CLASS( CWeaponMedikit, CWeaponSDKMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();
	
	CWeaponMedikit()
	{
	}

	virtual SDKWeaponID GetWeaponID( void ) const		{	return LF_WEAPON_MEDIKIT; }
	virtual float	GetRange( void )					{	return	64.0f;	}	//Tony; let the crowbar swing further.
	virtual bool CanWeaponBeDropped() const				{	return false; }

	virtual void PrimaryAttack( void );
	virtual void Heal( bool secondary );

	virtual void HealHit( trace_t &traceHit, Activity nHitActivity );

private:

	CWeaponMedikit( const CWeaponMedikit & );
};


#endif // WPN_MEDIKIT_H