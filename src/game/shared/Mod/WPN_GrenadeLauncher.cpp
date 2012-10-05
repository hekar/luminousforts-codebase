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
	Sourceforts
	Weapon

	Grenade Launcher that fires timed grenades
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#include "cbase.h"
#include "weapon_sdkbase.h"

#ifdef CLIENT_DLL

	#define CWeaponGrenadeLauncher C_WeaponGrenadeLauncher
	#include "c_sdk_player.h"

#else

	#include "sdk_player.h"
	#include "basegrenade_shared.h"
	#include "grenade_frag.h"

#endif

BALANCE_DEFINE( lfm_grenade_launcher_timer, 0.5 );
BALANCE_DEFINE( lfm_grenade_launcher_radius, 4.0 );
BALANCE_DEFINE( lfm_grenade_launcher_fire_delay, 1.0 );

class CWeaponGrenadeLauncher : public CWeaponSDKBase
{
public:
	DECLARE_CLASS( CWeaponGrenadeLauncher, CWeaponSDKBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();
	
	CWeaponGrenadeLauncher();

	void PrimaryAttack();
	virtual SDKWeaponID GetWeaponID( void ) const		{ return LF_WEAPON_GRENADE_LAUNCHER; }
	virtual int GetFireMode() const { return FM_SEMIAUTOMATIC; }

private:

	CWeaponGrenadeLauncher( const CWeaponGrenadeLauncher & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponGrenadeLauncher, DT_WeaponGrenadeLauncher )

BEGIN_NETWORK_TABLE( CWeaponGrenadeLauncher, DT_WeaponGrenadeLauncher )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponGrenadeLauncher )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_lf_grenade_launcher, CWeaponGrenadeLauncher );
PRECACHE_WEAPON_REGISTER( weapon_lf_grenade_launcher );



CWeaponGrenadeLauncher::CWeaponGrenadeLauncher()
{
}

void CWeaponGrenadeLauncher::PrimaryAttack()
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

	float radius = BALANCE_VALUE( Float, lfm_grenade_launcher_radius );

	trace_t tr;
	UTIL_TraceHull( vecEye, vecSrc, -Vector(radius+2,radius+2,radius+2), Vector(radius+2,radius+2,radius+2), 
		pPlayer->PhysicsSolidMaskForEntity(), pPlayer, pPlayer->GetCollisionGroup(), &tr );
	
	if ( tr.DidHit() )
	{
		vecSrc = tr.endpos;
	}

	vForward[0] += 0.01f;
	vForward[2] += 0.1f;

	float timer = BALANCE_VALUE( Float, lfm_grenade_launcher_timer );

	Vector vecThrow;
	pPlayer->GetVelocity( &vecThrow, NULL );
	vecThrow += vForward * 600;
	CBaseGrenade *pGrenade = Fraggrenade_Create( vecSrc, vec3_angle, vecThrow, AngularImpulse(600,random->RandomInt(-1200,1200),0), pPlayer, timer, false, GetSDKWpnData().m_flDamageForceScale );

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
		pGrenade->SetDamageRadius( radius );
	}
#endif

#ifdef GAME_DLL
	pPlayer->NoteWeaponFired();
#endif

	AddViewKick();

	m_iClip1--;

	m_flNextPrimaryAttack = BALANCE_VALUE( Float, lfm_grenade_launcher_fire_delay );
}

//Tony; todo; add ACT_MP_PRONE* activities, so we have them.
acttable_t CWeaponGrenadeLauncher::m_acttable[] = 
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

IMPLEMENT_ACTTABLE( CWeaponGrenadeLauncher );

