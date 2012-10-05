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

	A standard submachinegun
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#include "cbase.h"
#include "weapon_sdkbase.h"

#ifdef CLIENT_DLL
	#include "iviewrender_beams.h"
	#include "fx.h"
	#include "c_te_effect_dispatch.h"
#else
	#include "beam_shared.h"
#endif

#include "particle_parse.h"

#if defined( CLIENT_DLL )

	#define CWeaponSubMachineGun C_WeaponSubMachineGun
	#include "c_sdk_player.h"

#else

	#include "sdk_player.h"

#endif

// Primary fire
BALANCE_DEFINE( lfm_smg1_kick_min_x, 0.2 )
BALANCE_DEFINE( lfm_smg1_kick_min_y, 0.2 )
BALANCE_DEFINE( lfm_smg1_kick_min_z, 0.1 )

// View punch
BALANCE_DEFINE( lfm_smg1_vx_min, 0.25 )
BALANCE_DEFINE( lfm_smg1_vx_max, 0.5 )
BALANCE_DEFINE( lfm_smg1_vy_min, -0.6 )
BALANCE_DEFINE( lfm_smg1_vy_max, 0.6 )
BALANCE_DEFINE( lfm_smg1_view_punch, 10 )


class CWeaponSubMachineGun : public CWeaponSDKBase
{
public:
	DECLARE_CLASS( CWeaponSubMachineGun, CWeaponSDKBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();
	
	CWeaponSubMachineGun();

	void Precache();
	void PrimaryAttack();

	void AddViewKick();
	void DoMachineGunKick( CBasePlayer *pPlayer, float dampEasy, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime );

	virtual SDKWeaponID GetWeaponID( void ) const { return LF_WEAPON_SUBMACHINEGUN; }
	virtual int GetFireMode() const { return FM_AUTOMATIC; }

private:

	CWeaponSubMachineGun( const CWeaponSubMachineGun & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSubMachineGun, DT_WeaponSubMachineGun )

BEGIN_NETWORK_TABLE( CWeaponSubMachineGun, DT_WeaponSubMachineGun )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponSubMachineGun )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_lf_submachinegun, CWeaponSubMachineGun );
PRECACHE_WEAPON_REGISTER( weapon_lf_submachinegun );



CWeaponSubMachineGun::CWeaponSubMachineGun()
{
}

void CWeaponSubMachineGun::Precache()
{
}

void CWeaponSubMachineGun::PrimaryAttack()
{
	BaseClass::PrimaryAttack();

		// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if (!pPlayer)
		return;
	
	// Abort here to handle burst and auto fire modes
	if ( (UsesClipsForAmmo1() && m_iClip1 == 0) || ( !UsesClipsForAmmo1() && !pPlayer->GetAmmoCount(m_iPrimaryAmmoType) ) )
		return;

	AddViewKick();
}

void CWeaponSubMachineGun::AddViewKick( void )
{
	#define	EASY_DAMPEN			0.5f
	#define	MAX_VERTICAL_KICK	1.0f	//Degrees
	#define	SLIDE_LIMIT			2.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );

	// Add a view punch too
	QAngle viewPunch;
	viewPunch.x = SharedRandomFloat( "smg1pax", BALANCE_VALUE( Float, lfm_smg1_vx_min ), BALANCE_VALUE( Float, lfm_smg1_vx_max ) );
	viewPunch.y = SharedRandomFloat( "smg1pay", BALANCE_VALUE( Float, lfm_smg1_vy_min ), BALANCE_VALUE( Float, lfm_smg1_vy_max ) );
	viewPunch.z = 0.0f;
	pPlayer->ViewPunch( viewPunch );
}

void CWeaponSubMachineGun::DoMachineGunKick( CBasePlayer *pPlayer, float dampEasy, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime )
{
	#define	KICK_MIN_X			0.2f	//Degrees
	#define	KICK_MIN_Y			0.2f	//Degrees
	#define	KICK_MIN_Z			0.1f	//Degrees

	QAngle vecScratch;
	int iSeed = CBaseEntity::GetPredictionRandomSeed() & 255;
	
	//Find how far into our accuracy degradation we are
	float duration	= ( fireDurationTime > slideLimitTime ) ? slideLimitTime : fireDurationTime;
	float kickPerc = duration / slideLimitTime;

	// do this to get a hard discontinuity, clear out anything under 10 degrees punch
	pPlayer->ViewPunchReset( BALANCE_VALUE(Int, lfm_smg1_view_punch ) );

	//Apply this to the view angles as well
	vecScratch.x = -( BALANCE_VALUE( Float, lfm_smg1_kick_min_x ) + ( maxVerticleKickAngle * kickPerc ) );
	vecScratch.y = -( BALANCE_VALUE( Float, lfm_smg1_kick_min_y ) + ( maxVerticleKickAngle * kickPerc ) ) / 3;
	vecScratch.z = BALANCE_VALUE( Float, lfm_smg1_kick_min_z ) + ( maxVerticleKickAngle * kickPerc ) / 8;

	RandomSeed( iSeed );

	//Wibble left and right
	if ( RandomInt( -1, 1 ) >= 0 )
		vecScratch.y *= -1;

	iSeed++;

	//Wobble up and down
	if ( RandomInt( -1, 1 ) >= 0 )
		vecScratch.z *= -1;

	//Clip this to our desired min/max
	UTIL_ClipPunchAngleOffset( vecScratch, pPlayer->m_Local.m_vecPunchAngle, QAngle( 24.0f, 3.0f, 1.0f ) );

	//Add it to the view punch
	// NOTE: 0.5 is just tuned to match the old effect before the punch became simulated
	pPlayer->ViewPunch( vecScratch * 0.5 );
}


acttable_t CWeaponSubMachineGun::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_SMG1,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_SMG1,				false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_SMG1,						false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_SMG1,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_SMG1,			false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_SMG1,			false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_SMG1,					false },
};

IMPLEMENT_ACTTABLE( CWeaponSubMachineGun );

