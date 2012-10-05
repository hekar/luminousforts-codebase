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


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	Weapon
		FOR THE SCOUT
		AIRGUN BLASTER :OOOOOOOOOOOOOOOOOOOOO
		OH NOES THE AIRGUN, GUYS WE BETTER RUN!!!

	Last Updated June 9, 2010
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#include "cbase.h"
#include "npcevent.h"

#ifdef CLIENT_DLL
	#include "c_sdk_player.h"
	#include "c_te_effect_dispatch.h"
#else
	#include "sdk_player.h"
	#include "te_effect_dispatch.h"
	#include "prop_combine_ball.h"
#endif

#include "effect_dispatch_data.h"

#include "basegrenade_shared.h"
#include "weapon_sdkbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
#define CWeaponAirgun C_WeaponAirgun
#endif

BALANCE_DEFINE( lfm_weapon_airgun_accel, 7.0f );
BALANCE_DEFINE( lfm_weapon_airgun_accel_y, 11.0f );

/*==================================================================
	ClassDef
==================================================================*/
class CWeaponAirgun : public CWeaponSDKBase
{
	DECLARE_CLASS( CWeaponAirgun, CWeaponSDKBase );
	DECLARE_ACTTABLE();
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
public:
	CWeaponAirgun();

	void Precache( void );

	void ItemPostFrame();
	void PrimaryAttack();

	bool Reload() { return false; }

	SDKWeaponID GetWeaponID( void ) const { return LF_WEAPON_AIRBLASTER; }

	const char *GetTracerType( void ) { return "AirgunTracer"; }

private:
	CWeaponAirgun( const CWeaponAirgun & );

protected:
	float					m_flDelayedFire;
};

/*==================================================================
	Definitions
==================================================================*/
IMPLEMENT_NETWORKCLASS_ALIASED( WeaponAirgun, DT_WeaponAirgun )

BEGIN_NETWORK_TABLE( CWeaponAirgun, DT_WeaponAirgun )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponAirgun )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_lf_airgun, CWeaponAirgun );
PRECACHE_WEAPON_REGISTER(weapon_lf_airgun);


//
//	Name: CWeaponAirgun
//	Author: Mixture of Valve's code :/...
//	Description: hmmm...
//	Notes:
//
CWeaponAirgun::CWeaponAirgun( )
{
}

void CWeaponAirgun::Precache( void )
{
	BaseClass::Precache();
}

void CWeaponAirgun::ItemPostFrame( void )
{
	// Do something cool with the gun's vents?
	BaseClass::ItemPostFrame();
}

void CWeaponAirgun::PrimaryAttack()
{
	// TODO: Check player velocity to see if they're glitching!
	CSDKPlayer *pPlayer = ToSDKPlayer( GetOwner() );
	if ( !pPlayer )
		return;
	
	const QAngle& viewangles = pPlayer->EyeAngles();

	Vector viewvector;
	// Convert angle to vector
	AngleVectors( viewangles, &viewvector );

	VectorNormalize( viewvector );

	float horizontalAccel = 0;
	float verticalAccel = 0;
	if ( pPlayer->GetFlags() & FL_ONGROUND )
	{
		horizontalAccel *= BALANCE_VALUE( Float, lfm_weapon_airgun_accel ) * 0.05;
		verticalAccel *= BALANCE_VALUE( Float, lfm_weapon_airgun_accel_y ) * 0.15;
	}
	else
	{
		const Vector& vecStart = pPlayer->EyePosition();
		Vector vecEnd( vecStart.x, vecStart.y - 15000, vecStart.z );
		trace_t tr;
		UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID, pPlayer, COLLISION_GROUP_INTERACTIVE, &tr );
		
		float distance = abs(vecStart.z - tr.endpos.z) - pPlayer->EyePosition().z;
		// Height to stop clamping speed
		if ( distance < 1500 )
		{
			horizontalAccel = clamp(BALANCE_VALUE( Float, lfm_weapon_airgun_accel ) * (distance / 100), 
				0, BALANCE_VALUE( Float, lfm_weapon_airgun_accel ));
			verticalAccel = clamp(BALANCE_VALUE( Float, lfm_weapon_airgun_accel_y ) * (distance / 100),
				0, BALANCE_VALUE( Float, lfm_weapon_airgun_accel_y ));
		}
		else
		{
			horizontalAccel = BALANCE_VALUE( Float, lfm_weapon_airgun_accel );
			verticalAccel = BALANCE_VALUE( Float, lfm_weapon_airgun_accel_y );
		}
	}

	// Calculate the backwards accleration
	Vector accel = viewvector * Vector(
			horizontalAccel, 
			horizontalAccel,
			verticalAccel
	) * -1;
	// /* Accelerate backwards */

	// Just do a simple velocity change for now
	pPlayer->ApplyAbsVelocityImpulse( accel );
}

/*==================================================================
	Action Table
==================================================================*/
acttable_t	CWeaponAirgun::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_AR2,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_AR2,			false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_AR2,					false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_AR2,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_AR2,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_AR2,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_AR2,		false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_AR2,		false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_AR2,					false },
};

IMPLEMENT_ACTTABLE(CWeaponAirgun);
