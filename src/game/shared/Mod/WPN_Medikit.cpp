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
#include "weapon_sdkbase.h"
#include "sdk_weapon_melee.h"
#ifdef CLIENT_DLL
	#include "c_sdk_player.h"
#else
	#include "sdk_player.h"
	#include "ilagcompensationmanager.h"
#endif
#include "WPN_Medikit.h"

static const Vector g_HealMins(-MELEE_HULL_DIM,-MELEE_HULL_DIM,-MELEE_HULL_DIM);
static const Vector g_HealMaxs(MELEE_HULL_DIM,MELEE_HULL_DIM,MELEE_HULL_DIM);

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponMedikit, DT_WeaponMedikit )

BEGIN_NETWORK_TABLE( CWeaponMedikit, DT_WeaponMedikit )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponMedikit )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_lf_medikit, CWeaponMedikit );
PRECACHE_WEAPON_REGISTER( weapon_lf_medikit );

acttable_t CWeaponMedikit::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_MELEE,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_MELEE,			false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_MELEE,					false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_MELEE,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_MELEE,			false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_MELEE,			false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_MELEE,					false },
};

IMPLEMENT_ACTTABLE( CWeaponMedikit );

void CWeaponMedikit::PrimaryAttack()
{
	
#ifndef CLIENT_DLL
	CSDKPlayer *pPlayer = ToSDKPlayer( GetPlayerOwner() );
	pPlayer->NoteWeaponFired();
	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation( pPlayer, pPlayer->GetCurrentCommand() );
#endif
	Heal( false );

#ifndef CLIENT_DLL
	// Move other players back to history positions based on local player's lag
	lagcompensation->FinishLagCompensation( pPlayer );
#endif

}

void CWeaponMedikit::Heal( bool bIsSecondary )
{
	trace_t traceHit;

	// Try a ray
	CSDKPlayer *pOwner = ToSDKPlayer( GetOwner() );
	if ( !pOwner )
		return;

	Vector swingStart = pOwner->Weapon_ShootPosition( );
	Vector forward;

	pOwner->EyeVectors( &forward, NULL, NULL );

	Vector swingEnd = swingStart + forward * GetRange();
	UTIL_TraceLine( swingStart, swingEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &traceHit );
	Activity nHitActivity = ACT_VM_HITCENTER;

#ifndef CLIENT_DLL
	// Like bullets, melee traces have to trace against triggers.
	CTakeDamageInfo triggerInfo( GetOwner(), GetOwner(), GetDamageForActivity( nHitActivity ), DMG_CLUB );
	TraceAttackToTriggers( triggerInfo, traceHit.startpos, traceHit.endpos, vec3_origin );
#endif

	if ( traceHit.fraction == 1.0 )
	{
		float meleeHullRadius = 1.732f * MELEE_HULL_DIM;  // hull is +/- 16, so use cuberoot of 2 to determine how big the hull is from center to the corner point

		// Back off by hull "radius"
		swingEnd -= forward * meleeHullRadius;

		UTIL_TraceHull( swingStart, swingEnd, g_HealMins, g_HealMaxs, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &traceHit );
		if ( traceHit.fraction < 1.0 && traceHit.m_pEnt )
		{
			Vector vecToTarget = traceHit.m_pEnt->GetAbsOrigin() - swingStart;
			VectorNormalize( vecToTarget );

			float dot = vecToTarget.Dot( forward );

			// YWB:  Make sure they are sort of facing the guy at least...
			if ( dot < 0.70721f )
			{
				// Force amiss
				traceHit.fraction = 1.0f;
			}
			else
			{
				nHitActivity = ChooseIntersectionPointAndActivity( traceHit, g_HealMins, g_HealMaxs, pOwner );
			}
		}
	}

	WeaponSound( SINGLE );

	//	Miss
	if ( traceHit.fraction == 1.0f )
	{
		nHitActivity = bIsSecondary ? ACT_VM_MISSCENTER2 : ACT_VM_MISSCENTER;

		// We want to test the first swing again
		Vector testEnd = swingStart + forward * GetRange();
	}
	else
	{
		HealHit( traceHit, nHitActivity );
	}

	// Send the anim
	SendWeaponAnim( nHitActivity );

	pOwner->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

	//Setup our next attack times
	m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
	m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();
}


void CWeaponMedikit::HealHit( trace_t &traceHit, Activity nHitActivity )
{
	CSDKPlayer *pPlayer = ToSDKPlayer( GetOwner() );
	
	//Do view kick
	AddViewKick();

	CBaseEntity	*pHitEntity = traceHit.m_pEnt;

	//Apply damage to a hit target
	if ( pHitEntity != NULL )
	{
		if ( pHitEntity->IsPlayer() &&
			pHitEntity->GetTeamNumber() == pPlayer->GetTeamNumber() )
		{
			int health = pHitEntity->GetHealth();
			int maxHealth = pHitEntity->GetMaxHealth();
			int newHealth = min( health + GetDamageForActivity( nHitActivity ), maxHealth );
			pHitEntity->SetHealth( newHealth );
		}

		WeaponSound( MELEE_HIT );
	}

	// Apply an impact effect
	ImpactEffect( traceHit );
}
