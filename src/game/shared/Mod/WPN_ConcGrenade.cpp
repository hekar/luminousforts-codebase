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
	
	Concusion Grenade :/ (howto speel?)
	
	Last Updated June 9, 2010
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "cbase.h"
#include "weapon_sdkbase.h"
#include "gamerules.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"

#include "weapon_basesdkgrenade.h"


#ifdef CLIENT_DLL

#define CWeaponConc C_WeaponConc
	
#else

	#include "sdk_player.h"
	#include "items.h"
	#include "sdk_basegrenade_projectile.h"

#endif

class CWeaponConc : public CBaseSDKGrenade
{
public:
	DECLARE_CLASS( CWeaponConc, CBaseSDKGrenade );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

	CWeaponConc() {}

	virtual SDKWeaponID GetWeaponID( void ) const		{ return LF_WEAPON_CONC; }

#ifdef CLIENT_DLL

#else
	DECLARE_DATADESC();

	virtual void EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer, CWeaponSDKBase *pWeapon );
	
#endif

	CWeaponConc( const CWeaponConc & ) {}
};


#define GRENADE_TIMER	1.0f //Seconds

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponConc, DT_WeaponConc )

BEGIN_NETWORK_TABLE(CWeaponConc, DT_WeaponConc)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponConc )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_lf_conc, CWeaponConc );
PRECACHE_WEAPON_REGISTER( weapon_lf_conc );


#ifdef GAME_DLL

#define GRENADE_MODEL "models/Weapons/w_eq_fraggrenade_thrown.mdl"

class CConcProjectile : public CBaseGrenadeProjectile
{
public:
	DECLARE_CLASS( CConcProjectile, CBaseGrenadeProjectile );

	//Tony; by default projectiles don't have one, so make sure derived weapons do!!
	virtual SDKWeaponID GetWeaponID( void ) const		{ return LF_WEAPON_CONC; }

	// Overrides.
public:
	virtual void Spawn()
	{
		SetModel( GRENADE_MODEL );
		BaseClass::Spawn();
	}

	virtual void Precache()
	{
		PrecacheModel( GRENADE_MODEL );
		BaseClass::Precache();
	}

	virtual Vector GetBlastForce() { return Vector( 500.0f, 500.0f, 5000.0f ); }

	// Grenade stuff.
public:

	static CConcProjectile *Create( 
		const Vector &position, 
		const QAngle &angles, 
		const Vector &velocity, 
		const AngularImpulse &angVelocity, 
		CBaseCombatCharacter *pOwner,
		CWeaponSDKBase *pWeapon,
		float timer )
	{
		CConcProjectile *pGrenade = (CConcProjectile*)CBaseEntity::Create( "grenade_conc", position, angles, pOwner );

		// Set the timer for 1 second less than requested. We're going to issue a SOUND_DANGER
		// one second before detonation.
		pGrenade->SetVelocity( velocity, angVelocity );

		pGrenade->SetDetonateTimerLength( timer );
		pGrenade->SetAbsVelocity( velocity );
		pGrenade->SetupInitialTransmittedGrenadeVelocity( velocity );
		pGrenade->SetThrower( pOwner ); 

		pGrenade->SetGravity( BaseClass::GetGrenadeGravity() );
		pGrenade->SetFriction( BaseClass::GetGrenadeFriction() );
		pGrenade->SetElasticity( BaseClass::GetGrenadeElasticity() );

		pGrenade->m_flDamage = pWeapon->GetSDKWpnData().m_iDamage;
		pGrenade->m_DmgRadius = pGrenade->m_flDamage * 3.5f;
		pGrenade->ChangeTeam( pOwner->GetTeamNumber() );
		pGrenade->ApplyLocalAngularVelocityImpulse( angVelocity );	

		// make NPCs afaid of it while in the air
		pGrenade->SetThink( &CConcProjectile::DangerSoundThink );
		pGrenade->SetNextThink( gpGlobals->curtime );

		return pGrenade;
	}
};

LINK_ENTITY_TO_CLASS( grenade_conc, CConcProjectile );
PRECACHE_WEAPON_REGISTER( grenade_conc );

BEGIN_DATADESC( CWeaponConc )
END_DATADESC()

void CWeaponConc::EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer, CWeaponSDKBase *pWeapon )
{
	CConcProjectile::Create( vecSrc, vecAngles, vecVel, angImpulse, pPlayer, pWeapon, GRENADE_TIMER );
}
	
#endif

//Tony; todo; add ACT_MP_PRONE* activities, so we have them.
acttable_t CWeaponConc::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_GRENADE,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_GRENADE,			false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_GRENADE,					false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_GRENADE,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_GRENADE,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_GRENADE,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_GRENADE,		false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_GRENADE,		false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_GRENADE,					false },
};

IMPLEMENT_ACTTABLE( CWeaponConc );
