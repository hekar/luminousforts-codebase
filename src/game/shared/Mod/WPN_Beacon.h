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


#ifndef WPN_BEACON_H
#define WPN_BEACON_H

#ifdef WIN32
#pragma once
#endif // WIN32

#include "basegrenade_shared.h"
#include "weapon_sdkbase.h"

enum BeaconStates
{
	BEACON_TRIPMINE_READY,
	BEACON_SATCHEL_THROW,
	BEACON_SATCHEL_ATTACH
};

#ifdef CLIENT_DLL
#define CWeapon_Beacon C_Weapon_Beacon
#endif

class CWeapon_Beacon : public CWeaponSDKBase
{
	DECLARE_ACTTABLE();
#ifndef CLIENT_DLL
	DECLARE_DATADESC();
#endif

public:
	DECLARE_CLASS( CWeapon_Beacon, CWeaponSDKBase );

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CWeapon_Beacon();

	void Spawn( void );
	void Precache( void );

	void PrimaryAttack( void );
	void WeaponIdle( void );
	void Weapon_Switch( void );
	void BEACONThink( void );
	void StartSatchelThrow();
	void SatchelThrow();
	
	void SetPickupTouch( void );
	void BeaconTouch( CBaseEntity *pOther );	// default weapon touch
	void ItemPostFrame( void );	
	bool Reload( void );
	void SetBeaconState( int newState );
	bool CanAttachBEACON(void);		// In position where can attach BEACON?
	bool AnyUndetonatedCharges(void);
	void StartTripmineAttach( void );
	void TripmineAttach( void );

	void StartSatchelDetonate( void );
	void SatchelDetonate( void );
	void StartSatchelAttach( void );
	void SatchelAttach( void );
	bool Deploy( void );
	bool Holster( CBaseCombatWeapon *pSwitchingTo = NULL );

	void RemoveOldBeacons();

	virtual SDKWeaponID GetWeaponID( void ) const { return LF_WEAPON_BEACON; }

private:
	// Disallow Copying
	CWeapon_Beacon( const CWeapon_Beacon & );

	CNetworkVar( int,	m_tBeaconState );
	CNetworkVar( bool,	m_bNeedReload);
	CNetworkVar( bool,	m_bClearReload);
	CNetworkVar( bool,	m_bAttachSatchel);
	CNetworkVar( bool,	m_bAttachTripmine);
	CNetworkVar( bool,	m_bThrowSatchel);
	CNetworkVar( bool,	m_bBeaconPlaced);
	float				m_flWallSwitchTime;
};

#endif // WPN_BEACON_H