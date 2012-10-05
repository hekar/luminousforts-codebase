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

#include "cbase.h"
#if 0
#include "weapon_sdkbase.h"

#ifdef CLIENT_DLL

	#define CWeaponOrbitCannon C_WeaponOrbitCannon
	#include "c_sdk_player.h"

#else

	#include "sdk_player.h"
	#include "basegrenade_shared.h"
	#include "grenade_frag.h"

#endif

#define ORBIT_CANNON_TIMER 0.5
#define ORBIT_CANNON_RADIUS 4.0

#ifdef MOD_DEV
ConVar sdev_orbit_cannon_fire_delay( "sdev_orbit_cannon_fire_delay", "1.0", 0, "Delay for primary fire" );
#else
#define ORBIT_CANNON_FIRE_DELAY 1.0
#endif

class CWeaponOrbitCannon : public CWeaponSDKBase
{
public:
	DECLARE_CLASS( CWeaponOrbitCannon, CWeaponSDKBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();
	
	CWeaponOrbitCannon();

	void PrimaryAttack();
	virtual SDKWeaponID GetWeaponID( void ) const		{ return LF_WEAPON_ORBIT_CANNON; }
	virtual int GetFireMode() const { return FM_SEMIAUTOMATIC; }

private:

	CWeaponOrbitCannon( const CWeaponOrbitCannon & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponOrbitCannon, DT_WeaponOrbitCannon )

BEGIN_NETWORK_TABLE( CWeaponOrbitCannon, DT_WeaponOrbitCannon )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponOrbitCannon )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_lf_orbit_cannon, CWeaponOrbitCannon );
PRECACHE_WEAPON_REGISTER( weapon_lf_orbit_cannon );



CWeaponOrbitCannon::CWeaponOrbitCannon()
{
}

void CWeaponOrbitCannon::PrimaryAttack()
{
	// If my clip is empty (and I use clips) start reload
	if ( UsesClipsForAmmo1() && !m_iClip1 ) 
	{
		Reload();
		return;
	}

	CSDKPlayer *pPlayer = GetPlayerOwner();

	if (!pPlayer)
		return;

	// Fire nades
#ifndef CLIENT_DLL
	Vector	vecEye = pPlayer->EyePosition();
	Vector	vForward, vRight;

	pPlayer->EyeVectors( &vForward, &vRight, NULL );
	Vector vecSrc = vecEye + vForward * 18.0f + vRight * 8.0f;

	trace_t tr;
	UTIL_TraceHull( vecEye, vecSrc, -Vector(ORBIT_CANNON_RADIUS+2,ORBIT_CANNON_RADIUS+2,ORBIT_CANNON_RADIUS+2), Vector(ORBIT_CANNON_RADIUS+2,ORBIT_CANNON_RADIUS+2,ORBIT_CANNON_RADIUS+2), 
		pPlayer->PhysicsSolidMaskForEntity(), pPlayer, pPlayer->GetCollisionGroup(), &tr );
	
	if ( tr.DidHit() )
	{
		vecSrc = tr.endpos;
	}

	vForward[0] += 0.01f;
	vForward[2] += 0.1f;

	Vector vecThrow;
	pPlayer->GetVelocity( &vecThrow, NULL );
	vecThrow += vForward * 600;
	CBaseGrenade *pGrenade = Fraggrenade_Create( vecSrc, vec3_angle, vecThrow, AngularImpulse(600,random->RandomInt(-1200,1200),0), pPlayer, ORBIT_CANNON_TIMER, false, GetSDKWpnData().m_flDamageForceScale );

	if ( pGrenade )
	{
		if ( pPlayer && pPlayer->m_lifeState != LIFE_ALIVE )
		{
			pPlayer->GetVelocity( &vecThrow, NULL );

			IPhysicsObject *pPhysicsObject = pGrenade->VPhysicsGetObject();
			if ( pPhysicsObject )
			{
				pPhysicsObject->SetVelocity( &vecThrow, NULL );
			}
		}
		
		pGrenade->SetDamage( GetSDKWpnData().m_iDamage );
		pGrenade->SetDamageRadius( ORBIT_CANNON_RADIUS );
	}
#endif

#ifdef GAME_DLL
	pPlayer->NoteWeaponFired();
#endif

	AddViewKick();

	m_iClip1--;

#ifdef MOD_DEV
	m_flNextPrimaryAttack = gpGlobals->curtime + sdev_orbit_cannon_fire_delay.GetFloat();
#else
	m_flNextPrimaryAttack = gpGlobals->curtime + ORBIT_CANNON_FIRE_DELAY;
#endif
}

//Tony; todo; add ACT_MP_PRONE* activities, so we have them.
acttable_t CWeaponOrbitCannon::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_PISTOL,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_PISTOL,			false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_PISTOL,					false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_PISTOL,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_PISTOL,					false },
};

IMPLEMENT_ACTTABLE( CWeaponOrbitCannon );

#endif // 0