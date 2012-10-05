//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "gamerules.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "engine/IEngineSound.h"

#if defined( CLIENT_DLL )
#include "c_sdk_player.h"
#else
#include "sdk_player.h"
#include "grenade_satchel.h"
#include "entitylist.h"
#include "eventqueue.h"

#include "Proj_Beacon.h"
#endif

#include "WPN_Beacon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	BEACON_PRIMARY_VOLUME		450
#define BEACON_REFIRE_DELAY		0.05f

IMPLEMENT_NETWORKCLASS_ALIASED( Weapon_Beacon, DT_Weapon_Beacon )

	BEGIN_NETWORK_TABLE( CWeapon_Beacon, DT_Weapon_Beacon )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_tBeaconState ) ),
	RecvPropBool( RECVINFO( m_bNeedReload ) ),
	RecvPropBool( RECVINFO( m_bClearReload ) ),
	RecvPropBool( RECVINFO( m_bThrowSatchel ) ),
	RecvPropBool( RECVINFO( m_bAttachSatchel ) ),
	RecvPropBool( RECVINFO( m_bAttachTripmine ) ),
#else
	SendPropInt( SENDINFO( m_tBeaconState ) ),
	SendPropBool( SENDINFO( m_bNeedReload ) ),
	SendPropBool( SENDINFO( m_bClearReload ) ),
	SendPropBool( SENDINFO( m_bThrowSatchel ) ),
	SendPropBool( SENDINFO( m_bAttachSatchel ) ),
	SendPropBool( SENDINFO( m_bAttachTripmine ) ),
#endif
	END_NETWORK_TABLE()

#ifdef CLIENT_DLL

	BEGIN_PREDICTION_DATA( CWeapon_Beacon )
	DEFINE_PRED_FIELD( m_tBeaconState, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bNeedReload, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bClearReload, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bThrowSatchel, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bAttachSatchel, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bAttachTripmine, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	END_PREDICTION_DATA()

#endif

	LINK_ENTITY_TO_CLASS( weapon_lf_beacon, CWeapon_Beacon );
PRECACHE_WEAPON_REGISTER(weapon_lf_beacon);

#ifndef CLIENT_DLL

BEGIN_DATADESC( CWeapon_Beacon )

	DEFINE_FIELD( m_tBeaconState, FIELD_INTEGER ),
	DEFINE_FIELD( m_bNeedReload, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bClearReload, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bThrowSatchel, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bAttachSatchel, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bAttachTripmine, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flWallSwitchTime, FIELD_TIME ),

	// Function Pointers
	DEFINE_FUNCTION( BeaconTouch ),

	END_DATADESC()
#endif

	acttable_t	CWeapon_Beacon::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_SLAM,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_SLAM,				false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_SLAM,						false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_SLAM,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SLAM,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SLAM,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_SLAM,			false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_SLAM,			false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_SLAM,					false },
};

IMPLEMENT_ACTTABLE(CWeapon_Beacon);

CWeapon_Beacon::CWeapon_Beacon(void)
{
	m_tBeaconState			= (int)BEACON_SATCHEL_THROW;
	m_bNeedReload			= true;
	m_bClearReload			= false;
	m_bThrowSatchel			= false;
	m_bAttachSatchel		= false;
	m_bAttachTripmine		= false;
	m_bBeaconPlaced = false;	
}

void CWeapon_Beacon::Spawn( )
{
	m_iPrimaryAmmoType = -1;
	m_iSecondaryAmmoType = -1;
	BaseClass::Spawn();
	m_iPrimaryAmmoType = -1;
	m_iSecondaryAmmoType = -1;


	Precache( );

#ifndef CLIENT_DLL
	FallInit();// get ready to fall down
#endif

	m_tBeaconState		= (int)BEACON_SATCHEL_THROW;
	m_flWallSwitchTime	= 0;

	// Give 1 piece of default ammo when first picked up
	m_iClip1 = -1;
	m_iClip2 = -2;
}

void CWeapon_Beacon::Precache( void )
{
	BaseClass::Precache();

#ifndef CLIENT_DLL
	UTIL_PrecacheOther( "npc_beacon" );
	UTIL_PrecacheOther( "npc_tripmine" );
	UTIL_PrecacheOther( "npc_satchel" );
#endif

	PrecacheScriptSound( "Weapon_Beacon.TripMineMode" );
	PrecacheScriptSound( "Weapon_Beacon.SatchelDetonate" );
	PrecacheScriptSound( "Weapon_Beacon.SatchelThrow" );
}

void CWeapon_Beacon::SetPickupTouch( void )
{
	SetTouch(&CWeapon_Beacon::BeaconTouch);
}

void CWeapon_Beacon::BeaconTouch( CBaseEntity *pOther )
{
#ifdef GAME_DLL
	CBaseCombatCharacter* pBCC = ToBaseCombatCharacter( pOther );

	// Can I even pick stuff up?
	if ( pBCC && !pBCC->IsAllowedToPickupWeapons() )
		return;
#endif

	// ---------------------------------------------------
	//  First give weapon to touching entity if allowed
	// ---------------------------------------------------
	BaseClass::DefaultTouch(pOther);
}

bool CWeapon_Beacon::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	SetThink(NULL);
	return BaseClass::Holster(pSwitchingTo);
}

bool CWeapon_Beacon::Reload( void )
{
	WeaponIdle( );
	return true;
}

void CWeapon_Beacon::PrimaryAttack( void )
{
	CBaseCombatCharacter *pOwner  = GetOwner();
	if (!pOwner)
	{ 
		return;
	}

	switch (m_tBeaconState)
	{
	case BEACON_TRIPMINE_READY:
		if (CanAttachBEACON())
		{
			StartTripmineAttach();
		}
		break;
	case BEACON_SATCHEL_THROW:
		StartSatchelThrow();
		break;
	case BEACON_SATCHEL_ATTACH:
		StartSatchelAttach();
		break;
	}
}

void CWeapon_Beacon::SatchelDetonate()
{
}

bool CWeapon_Beacon::AnyUndetonatedCharges(void)
{
	return false;
}

void CWeapon_Beacon::StartSatchelDetonate()
{
	// needs a higher delay on all of these, a minimum time really - to elimiate refires.
	m_flNextPrimaryAttack	= m_flNextSecondaryAttack = BEACON_REFIRE_DELAY + gpGlobals->curtime + SequenceDuration();
}

void CWeapon_Beacon::TripmineAttach( void )
{
	CSDKPlayer *pOwner  = ToSDKPlayer( GetOwner() );
	if (!pOwner)
	{
		return;
	}

	m_bAttachTripmine = false;

	Vector vecSrc, vecAiming;

	// Take the eye position and direction
	vecSrc = pOwner->EyePosition();

	QAngle angles = pOwner->GetLocalAngles();

	AngleVectors( angles, &vecAiming );

	trace_t tr;

	UTIL_TraceLine( vecSrc, vecSrc + (vecAiming * 128), MASK_SOLID, pOwner, COLLISION_GROUP_NONE, &tr );

	if (tr.fraction < 1.0)
	{
		CBaseEntity *pEntity = tr.m_pEnt;
		if (pEntity && !(pEntity->GetFlags() & FL_CONVEYOR))
		{
			RemoveOldBeacons();
#ifndef CLIENT_DLL
			QAngle angles;
			VectorAngles(tr.plane.normal, angles);

			angles.x += 90;

			CBaseEntity *pEnt = CBaseEntity::Create( "npc_beacon", tr.endpos + tr.plane.normal * 3, angles, NULL );

			CBeaconGrenade *pMine = (CBeaconGrenade *)pEnt;
			pMine->m_hOwner = GetOwner();
			// Attempt to attach to entity, or just sit still in place.
			pMine->AttachToEntity( pEntity );
#endif
		}
	}
}

void CWeapon_Beacon::StartTripmineAttach( void )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if (!pPlayer)
	{
		return;
	}

	Vector vecSrc, vecAiming;

	// Take the eye position and direction
	vecSrc = pPlayer->EyePosition();

	QAngle angles = pPlayer->GetLocalAngles();

	AngleVectors( angles, &vecAiming );

	trace_t tr;

	UTIL_TraceLine( vecSrc, vecSrc + (vecAiming * 128), MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr );

	if (tr.fraction < 1.0)
	{
		// ALERT( at_console, "hit %f\n", tr.flFraction );

		CBaseEntity *pEntity = tr.m_pEnt;
		if (pEntity && !(pEntity->GetFlags() & FL_CONVEYOR))
		{
			// player "shoot" animation
			pPlayer->SetAnimation( PLAYER_ATTACK1 );
			//Tony: ???
			ToSDKPlayer(pPlayer)->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );


			// -----------------------------------------
			//  Play attach animation
			// -----------------------------------------
			SendWeaponAnim(ACT_SLAM_TRIPMINE_ATTACH);

			m_bNeedReload		= false;
			m_bAttachTripmine	= true;
			m_bBeaconPlaced = true;
		}
		else
		{
			// ALERT( at_console, "no deploy\n" );
		}
	}

	// needs a higher delay on all of these, a minimum time really - to elimiate refires.
	m_flNextPrimaryAttack	= m_flNextSecondaryAttack = BEACON_REFIRE_DELAY + gpGlobals->curtime + SequenceDuration();
	//	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );
}

void CWeapon_Beacon::SatchelThrow( void )
{	
}

void CWeapon_Beacon::StartSatchelThrow( void )
{
}

void CWeapon_Beacon::SatchelAttach( void )
{
#ifndef CLIENT_DLL
	CBaseCombatCharacter *pOwner  = GetOwner();
	if (!pOwner)
	{
		return;
	}

	m_bAttachSatchel = false;

	Vector vecSrc	 = pOwner->Weapon_ShootPosition( );
	Vector vecAiming = pOwner->BodyDirection2D( );

	trace_t tr;

	UTIL_TraceLine( vecSrc, vecSrc + (vecAiming * 128), MASK_SOLID, pOwner, COLLISION_GROUP_NONE, &tr );

	if (tr.fraction < 1.0)
	{
		CBaseEntity *pEntity = tr.m_pEnt;
		if (pEntity && !(pEntity->GetFlags() & FL_CONVEYOR))
		{
			QAngle angles;
			VectorAngles(tr.plane.normal, angles);
			angles.y -= 90;
			angles.z -= 90;
			tr.endpos.z -= 6.0f;

			CSatchelCharge *pSatchel	= (CSatchelCharge*)CBaseEntity::Create( "npc_satchel", tr.endpos + tr.plane.normal * 3, angles, NULL );
			pSatchel->SetMoveType( MOVETYPE_FLY ); // no gravity
			pSatchel->m_bIsAttached		= true;
			pSatchel->m_bIsLive			= true;
			pSatchel->SetThrower( GetOwner() );
			pSatchel->SetOwnerEntity( ((CBaseEntity*)GetOwner()) );
			//pSatchel->m_pMyWeaponSLAM	= this;
		}
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CWeapon_Beacon::StartSatchelAttach( void )
{
#ifndef CLIENT_DLL
	CBaseCombatCharacter *pOwner  = GetOwner();
	if (!pOwner)
	{
		return;
	}

	Vector vecSrc	 = pOwner->Weapon_ShootPosition( );
	Vector vecAiming = pOwner->BodyDirection2D( );

	trace_t tr;

	UTIL_TraceLine( vecSrc, vecSrc + (vecAiming * 128), MASK_SOLID, pOwner, COLLISION_GROUP_NONE, &tr );

	if (tr.fraction < 1.0)
	{
		CBaseEntity *pEntity = tr.m_pEnt;
		if (pEntity && !(pEntity->GetFlags() & FL_CONVEYOR))
		{
			// Only the player fires this way so we can cast
			CBasePlayer *pPlayer = ToBasePlayer( pOwner );

			// player "shoot" animation
			pPlayer->SetAnimation( PLAYER_ATTACK1 );
			//Tony; need to check the player models !
			ToSDKPlayer(pPlayer)->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );


			// -----------------------------------------
			//  Play attach animation
			// -----------------------------------------
			SendWeaponAnim(ACT_SLAM_STICKWALL_ND_ATTACH);

			m_bNeedReload		= false;
			m_bAttachSatchel	= true;

			m_flNextPrimaryAttack	= m_flNextSecondaryAttack = BEACON_REFIRE_DELAY + gpGlobals->curtime + SequenceDuration();

		}
	}
#endif
}

void CWeapon_Beacon::SetBeaconState( int newState )
{
	// Set set and set idle time so animation gets updated with state change
	m_tBeaconState = newState;
	SetWeaponIdleTime( gpGlobals->curtime );
}

void CWeapon_Beacon::BEACONThink( void )
{
	if ( m_flWallSwitchTime > gpGlobals->curtime )
		return;

	if (CanAttachBEACON())
	{
		if (m_tBeaconState == BEACON_SATCHEL_THROW)
		{
			SetBeaconState(BEACON_TRIPMINE_READY);
			int iAnim =	ACT_SLAM_THROW_TO_TRIPMINE_ND;
			SendWeaponAnim( iAnim );
			m_flWallSwitchTime = gpGlobals->curtime + SequenceDuration();
			m_bNeedReload = false;
		}
	}
	else
	{
		if (m_tBeaconState == BEACON_TRIPMINE_READY)
		{
			SetBeaconState(BEACON_SATCHEL_THROW);
			int iAnim =	ACT_SLAM_TRIPMINE_TO_THROW_ND;
			SendWeaponAnim( iAnim );
			m_flWallSwitchTime = gpGlobals->curtime + SequenceDuration();
			m_bNeedReload = false;
		}
	}
}

bool CWeapon_Beacon::CanAttachBEACON( void )
{
	CSDKPlayer *pOwner = ToSDKPlayer( GetOwner() );

	if (!pOwner)
	{
		return false;
	}

	Vector vecSrc, vecAiming;

	// Take the eye position and direction
	vecSrc = pOwner->EyePosition();

	QAngle angles = pOwner->GetLocalAngles();

	AngleVectors( angles, &vecAiming );

	trace_t tr;

	Vector	vecEnd = vecSrc + (vecAiming * 42);
	UTIL_TraceLine( vecSrc, vecEnd, MASK_SOLID, pOwner, COLLISION_GROUP_NONE, &tr );

	if (tr.fraction < 1.0)
	{
		// Don't attach to a living creature
		if (tr.m_pEnt)
		{
			CBaseEntity *pEntity = tr.m_pEnt;
			CBaseCombatCharacter *pBCC		= ToBaseCombatCharacter( pEntity );
			if (pBCC)
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

void CWeapon_Beacon::ItemPostFrame( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if (!pOwner)
	{
		return;
	}

	BEACONThink();

	if ((pOwner->m_nButtons & IN_ATTACK2) && (m_flNextSecondaryAttack <= gpGlobals->curtime))
	{
		SecondaryAttack();
	}
	else if (!m_bNeedReload && (pOwner->m_nButtons & IN_ATTACK) && (m_flNextPrimaryAttack <= gpGlobals->curtime))
	{
		PrimaryAttack();
	}

	// -----------------------
	//  No buttons down
	// -----------------------
	else 
	{
		WeaponIdle( );
		return;
	}
}

void CWeapon_Beacon::Weapon_Switch( void )
{  
}

void CWeapon_Beacon::WeaponIdle( void )
{
	// Ready to switch animations?
	if ( HasWeaponIdleTimeElapsed() )
	{
		// Don't allow throw to attach switch unless in idle
		if (m_bClearReload)
		{
			m_bNeedReload  = false;
			m_bClearReload = false;
		}
		CBaseCombatCharacter *pOwner  = GetOwner();
		if (!pOwner)
		{
			return;
		}

		int iAnim = 0;

		if (m_bAttachSatchel)
		{
			SatchelAttach();
			iAnim = ACT_SLAM_STICKWALL_ND_ATTACH2;
		}
		else if (m_bAttachTripmine)
		{
			TripmineAttach();
			iAnim = ACT_SLAM_TRIPMINE_ATTACH2;
		}	
		else if ( m_bNeedReload )
		{	
			// If owner had ammo draw the correct BEACON type
			if ( m_bBeaconPlaced )
			{
				switch( m_tBeaconState)
				{
				case BEACON_TRIPMINE_READY:
					{
						iAnim = ACT_SLAM_TRIPMINE_DRAW;
					}
					break;
				case BEACON_SATCHEL_ATTACH:
					{
						iAnim =	ACT_SLAM_STICKWALL_ND_DRAW;
					}
					break;
				case BEACON_SATCHEL_THROW:
					{
						iAnim =	ACT_SLAM_THROW_ND_DRAW;
					}
					break;
				}
				m_bClearReload			= true;
			}
		}

		// If I don't need to reload just do the appropriate idle
		else
		{
			switch( m_tBeaconState)
			{
			case BEACON_TRIPMINE_READY:
				{
					iAnim = ACT_SLAM_TRIPMINE_IDLE;
					m_flWallSwitchTime = 0;
				}
				break;
			case BEACON_SATCHEL_THROW:
				{
					iAnim = ACT_SLAM_THROW_ND_IDLE;
					m_flWallSwitchTime = 0;
				}
				break;
			case BEACON_SATCHEL_ATTACH:
				{
					iAnim = ACT_SLAM_TRIPMINE_IDLE;
					m_flWallSwitchTime = 0;
				}
				break;
			}
		}
		SendWeaponAnim( iAnim );
	}
}

bool CWeapon_Beacon::Deploy( void )
{
	CBaseCombatCharacter *pOwner  = GetOwner();
	if (!pOwner)
	{
		return false;
	}

	SetModel( GetViewModel() );

	m_tBeaconState		= (int)BEACON_SATCHEL_THROW;

	// ------------------------------
	// Pick the right draw animation
	// ------------------------------
	int iActivity;

	m_bNeedReload = false;
	if (CanAttachBEACON())
	{
		iActivity = ACT_SLAM_TRIPMINE_DRAW; 
		SetBeaconState(BEACON_TRIPMINE_READY);
	}
	else
	{
		iActivity = ACT_SLAM_THROW_ND_DRAW; 
		SetBeaconState(BEACON_SATCHEL_THROW);
	}

	return DefaultDeploy( (char*)GetViewModel(), (char*)GetWorldModel(), iActivity, (char*)GetAnimPrefix() );
}

void CWeapon_Beacon::RemoveOldBeacons()
{
#ifndef CLIENT_DLL
	CBaseEntity *pEntity = NULL;

	while ((pEntity = gEntList.FindEntityByClassname( pEntity, "npc_beacon" )) != NULL)
	{
		CBeaconGrenade *pBeacon = dynamic_cast<CBeaconGrenade *>(pEntity);
		if ( pBeacon->m_hOwner == GetOwner() )
		{
			UTIL_Remove( pBeacon );
		}
	}
#endif // CLIENT_DLL
}