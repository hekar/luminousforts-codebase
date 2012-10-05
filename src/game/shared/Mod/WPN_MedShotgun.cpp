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

	Medium Shotgun
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#include "cbase.h"
#include "weapon_sdkbase.h"
#include "sdk_fx_shared.h"


#if defined( CLIENT_DLL )

	#define CWeaponMediumShotgun C_WeaponMediumShotgun
	#include "c_sdk_player.h"

#else

	#include "sdk_player.h"
	#include "te_firebullets.h"

#endif


class CWeaponMediumShotgun : public CWeaponSDKBase
{
public:
	DECLARE_CLASS( CWeaponMediumShotgun, CWeaponSDKBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

	CWeaponMediumShotgun();

	virtual void PrimaryAttack();
	virtual bool Reload();
	virtual void WeaponIdle();

	virtual SDKWeaponID GetWeaponID( void ) const		{ return LF_WEAPON_MEDIUMSHOTGUN; }
	virtual float GetWeaponSpread() { return 0.08f; }


private:

	CWeaponMediumShotgun( const CWeaponMediumShotgun & );

	CNetworkVar( int, m_iInSpecialReload );

};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponMediumShotgun, DT_WeaponMediumShotgun )

BEGIN_NETWORK_TABLE( CWeaponMediumShotgun, DT_WeaponMediumShotgun )

	#ifdef CLIENT_DLL
		RecvPropInt( RECVINFO( m_iInSpecialReload ) )
	#else
		SendPropInt( SENDINFO( m_iInSpecialReload ), 2, SPROP_UNSIGNED )
	#endif

END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponMediumShotgun )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_iInSpecialReload, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
#endif
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_lf_medium_shotgun, CWeaponMediumShotgun );
PRECACHE_WEAPON_REGISTER( weapon_lf_medium_shotgun );

CWeaponMediumShotgun::CWeaponMediumShotgun()
{
}

void CWeaponMediumShotgun::PrimaryAttack()
{
	CSDKPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	// don't fire underwater
	if (pPlayer->GetWaterLevel() == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = gpGlobals->curtime + 0.15;
		return;
	}

	BaseClass::PrimaryAttack();
}


bool CWeaponMediumShotgun::Reload()
{
	CSDKPlayer *pPlayer = GetPlayerOwner();

	if (pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 || m_iClip1 == GetMaxClip1())
		return true;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > gpGlobals->curtime)
		return true;
		
	// check to see if we're ready to reload
	if (m_iInSpecialReload == 0)
	{
		pPlayer->SetAnimation( PLAYER_RELOAD );

		SendWeaponAnim( ACT_SHOTGUN_RELOAD_START );
		m_iInSpecialReload = 1;
		pPlayer->m_flNextAttack = gpGlobals->curtime + 0.5;
		m_flNextPrimaryAttack = gpGlobals->curtime + 0.5;
		m_flNextSecondaryAttack = gpGlobals->curtime + 0.5;
		SetWeaponIdleTime( gpGlobals->curtime + 0.5 );
		return true;
	}
	else if (m_iInSpecialReload == 1) // Cocking gun
	{
		if (m_flTimeWeaponIdle > gpGlobals->curtime)
			return true;

		// was waiting for gun to move to side
		m_iInSpecialReload = 2;

		SendWeaponAnim( ACT_VM_RELOAD );
		SetWeaponIdleTime( gpGlobals->curtime + 0.45 );
	}
	else if ( m_iInSpecialReload == 2 ) // Actual reloading // Sanity, make sure it's actually in the right state.
	{
		// Add them to the clip
		m_iClip1 += 1;
		
#ifdef GAME_DLL
		SendReloadEvents();
#endif
		CSDKPlayer *pPlayer = GetPlayerOwner();

		if ( pPlayer )
			 pPlayer->RemoveAmmo( 1, m_iPrimaryAmmoType );

		m_iInSpecialReload = 1;
	}

	return true;
}


void CWeaponMediumShotgun::WeaponIdle()
{
	CSDKPlayer *pPlayer = GetPlayerOwner();

	if (m_flTimeWeaponIdle < gpGlobals->curtime)
	{
		// Dirty hack
		if (m_iClip1 != 6 && pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) )
		{
			Reload();
		}
		else
		{
			SendWeaponAnim( ACT_VM_IDLE );
		}
	}
}

// Acttables
acttable_t CWeaponMediumShotgun::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_SHOTGUN,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_SHOTGUN,			false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_SHOTGUN,					false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_SHOTGUN,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_SHOTGUN,		false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_SHOTGUN,		false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_SHOTGUN,					false },
};

IMPLEMENT_ACTTABLE( CWeaponMediumShotgun );
