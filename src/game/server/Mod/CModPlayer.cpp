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


/*===============================================================
	Server
	
	The Player class that introduces modifications.
	Unfortunately, most of the modifications are actually in
	CSDKPlayer and this piece of code hasn't been looked at 
	in a long time.

	Though, this is still the instantiated class for players.
	If you have time, think about moving some mod specific
	code from CSDKPlayer to CModPlayer.

	Last Updated June 21, 2010
===============================================================*/

#include "cbase.h"

#ifdef MOD_ALPHA_PLAYER
#include "sdk_team.h"
#include "sdk_player.h"

#ifdef MOD_SF132
#include "SF132_GameRules.h"
#endif

#include "props.h"
#include "grenade_satchel.h"
#include "eventqueue.h"
#include "igameevents.h"
#include "GameEventListener.h"

#include "CBlockBase.h"
#include "WPN_CombatCannon.h"
#include "CFlagBase.h"
#include "Proj_Beacon.h"
#include "ClassicGameRules.h"
#include "CModPlayer.h"
#include "CModPlayerShared.h"

LINK_ENTITY_TO_CLASS( player, CModPlayer );
PRECACHE_REGISTER( player );

BEGIN_DATADESC( CModPlayer )
END_DATADESC()

// ModPlayerShared Data Tables
//=============================
// specific to the local player
BEGIN_SEND_TABLE_NOBASE( CModPlayerShared, DT_ModPlayerShared )
END_SEND_TABLE()

// main table
IMPLEMENT_SERVERCLASS_ST( CModPlayer, DT_ModPlayer )
END_SEND_TABLE()

CModPlayer::CModPlayer()
{
	m_bHasFlag = false;
	m_bHasTouched = false;
	m_pFlag = NULL;
}

CModPlayer::~CModPlayer()
{
}

CModPlayer *CModPlayer::CreatePlayer( const char *className, edict_t *ed )
{
	CModPlayer::s_PlayerEdict = ed;
	return ( CModPlayer* )CreateEntityByName( className );
}

CFlagBase *CModPlayer::GetFlag()
{
	return ( CFlagBase * ) m_pFlag;
}

bool CModPlayer::HasTouched()
{
	return m_bHasTouched;
}

// Has touched flag
void CModPlayer::SetTouched( bool touched )
{
	m_bHasTouched = touched;
}

void CModPlayer::GiveFlag (CBaseEntity *pFlag)
{
	if ( !pFlag )
	{
		return;
	}

	m_pFlag = dynamic_cast< CFlagBase * > (pFlag);
	m_bHasFlag = true;

	m_pFlag->Pickup( this );
}

bool CModPlayer::HasFlag()
{
	return m_bHasFlag;
}

void CModPlayer::ReturnFlag( bool scored )
{
	if (!m_pFlag || !m_bHasFlag)
	{
		return;
	}

	m_pFlag->ReturnToSpawn( scored );

	m_bHasFlag = false;
	m_pFlag = NULL;
}

void CModPlayer::DropFlag()
{
	if ( !m_pFlag || !m_bHasFlag )
	{
		return;
	}

	m_pFlag->Drop();

	m_bHasFlag = false;
	m_pFlag = NULL;
}

void CModPlayer::ChangeTeam( int iTeamNum )
{
	if ( iTeamNum == TEAM_SPECTATOR )
	{
		DropFlag();
	}

	BaseClass::ChangeTeam( iTeamNum );
}

void CModPlayer::Event_Active()
{
	BaseClass::Event_Active();
}

void CModPlayer::Event_Disconnected()
{
	if ( HasFlag() )
	{
		DropFlag();
	}

	m_bHasTouched = false;

	// Remove all the player's beacons
	RemoveBeacons();

	BaseClass::Event_Disconnected();
}

void CModPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	if ( HasFlag() )
	{
		DropFlag();
	}

	m_bHasTouched = false;

	// Detonate all the slams!
	DetonateTripmines();

	// Allow the player to sprint again
	m_Shared.SetAllowedToSprint( true );

	CWeaponSDKBase *wpn = GetActiveSDKWeapon();
	if ( wpn && wpn->GetWeaponID() == LF_WEAPON_COMBATCANNON )
	{
		CWeaponCombatCannon *combatcannon = dynamic_cast< CWeaponCombatCannon* >( wpn );
		assert( combatcannon );
		if ( combatcannon )
		{
			DevMsg( "Reset block freezing\n" );
			combatcannon->ResetFreezing( false );

			CBlockBase *block = combatcannon->GetLastBlock();
			if ( block )
			{
				DevMsg( "Block current freezer is set to NULL\n" );
				block->SetCurrentFreezer( NULL );
			}
		}
	}

	BaseClass::Event_Killed( info );
}

void CModPlayer::Precache()
{
	UTIL_PrecacheOther( "lfc_prop_block1x2" );
	UTIL_PrecacheOther( "lfc_prop_block1x3" );
	UTIL_PrecacheOther( "lfc_prop_block1x4" );
	UTIL_PrecacheOther( "lfc_prop_block1x5" );
	UTIL_PrecacheOther( "lfc_prop_block2x2" );
	UTIL_PrecacheOther( "lfc_prop_block2x3" );
	UTIL_PrecacheOther( "lfc_prop_block2x4" );
	UTIL_PrecacheOther( "lfc_prop_block2x5" );
	UTIL_PrecacheOther( "lfc_prop_block3x3" );
	UTIL_PrecacheOther( "lfc_prop_block3x4" );
	UTIL_PrecacheOther( "lfc_prop_block3x5" );
	PrecacheScriptSound( "HL2Player.SprintStart" );
	BaseClass::Precache();
}

void CModPlayer::Spawn()
{
	BaseClass::Spawn();

	// Send Flag info if in classic mode
	if ( GameRules()->GetGameModeMask() & GAMEMODE_CLASSIC )
	{
#ifndef CLIENT_DLL
		//HODO: fix this, because could cause lag
		// Refresh the phase time for everyone
		ClassicGameRules()->SetPhaseTime( ClassicGameRules()->GetPhaseTimeLeft() );
#endif // !CLIENT_DLL

		CBaseEntity *ent = NULL;
		while ( ( ent = gEntList.FindEntityByClassname( ent, "lfc_prop_flag" ) ) != NULL )
		{
			if ( ent->IsFlag() )
			{
				CFlagBase *flag = dynamic_cast< CFlagBase *> ( ent );
				flag->UpdateFlagStatus( this );
			}
		}
	}

#ifdef MOD_SF132
	if ( GameRules()->GetGameMode() == GAMEMODE_SF132 )
	{
		if ( SF132GameRules()->GetCurrentPhaseID() == PHASE_COMBAT )
		{
			m_ItemRegister.GiveItems( this );
		}
	}
#endif // MOD_SF132
}

void CModPlayer::DetonateTripmines()
{
	CBaseEntity *pEntity = NULL;

	while ((pEntity = gEntList.FindEntityByClassname( pEntity, "npc_satchel" )) != NULL)
	{
		CSatchelCharge *pSatchel = dynamic_cast<CSatchelCharge *>(pEntity);
		if (pSatchel->m_bIsLive && pSatchel->GetThrower() == this )
		{
			g_EventQueue.AddEvent( pSatchel, "Explode", 0.20, this, this );
		}
	}

	// Play sound for pressing the detonator
	EmitSound( "Weapon_SLAM.SatchelDetonate" );
}

void CModPlayer::RemoveBeacons()
{
#ifndef CLIENT_DLL
	CBaseEntity *pEntity = NULL;

	while ((pEntity = gEntList.FindEntityByClassname( pEntity, "npc_beacon" )) != NULL)
	{
		CBeaconGrenade *pBeacon = dynamic_cast<CBeaconGrenade *>(pEntity);
		if ( pBeacon->m_hOwner == this )
		{
			UTIL_Remove( pBeacon );
		}
	}
#endif // CLIENT_DLL
}

CBaseEntity *CModPlayer::EntSelectSpawnPoint()
{
	CBaseEntity *pSpot = NULL;
	const char *pSpawnPointName = "";

	switch( GetTeamNumber() )
	{
#if defined ( SDK_USE_TEAMS )
	case SDK_TEAM_BLUE:
		{
			pSpot = g_pLastBlueSpawn;
			if ( SelectTeamSpawnSpot( SDK_TEAM_BLUE, pSpot ) )
			{
				g_pLastBlueSpawn = pSpot;
			}
		}
		break;
	case SDK_TEAM_RED:
		{
			pSpot = g_pLastRedSpawn;
			if ( SelectTeamSpawnSpot( SDK_TEAM_RED, pSpot ) )
			{
				g_pLastRedSpawn = pSpot;
			}
		}		
		break;
#endif // SDK_USE_TEAMS
	case TEAM_UNASSIGNED:
		{
			pSpawnPointName = "info_player_deathmatch";
			pSpot = g_pLastDMSpawn;
			if ( SelectSpawnSpot( pSpawnPointName, pSpot ) )
			{
				g_pLastDMSpawn = pSpot;
			}
		}		
		break;
	case TEAM_SPECTATOR:
	default:
		{
			pSpot = CBaseEntity::Instance( INDEXENT(0) );
		}
		break;		
	}

	if ( !pSpot )
	{
		Warning( "PutClientInServer: no %s on level\n", pSpawnPointName );
		return CBaseEntity::Instance( INDEXENT(0) );
	}

	return pSpot;
}

bool CModPlayer::SelectTeamSpawnSpot( int iTeamNum, CBaseEntity* &pSpot )
{
	CBaseEntity *pNewSpot = NULL;
	for ( int i = 0; i < ARRAYSIZE( g_GenericSpawnPoints ); i++ )
	{
		const char *GenericSpawnPointName = g_GenericSpawnPoints[ i ];

		pNewSpot = gEntList.FindEntityByClassname( pSpot, GenericSpawnPointName );

		if ( pNewSpot == NULL ) // skip over the null point
			pNewSpot = gEntList.FindEntityByClassname( pNewSpot, GenericSpawnPointName );

		CBaseEntity *pFirstSpot = pNewSpot;
		do
		{
			if ( pNewSpot )
			{
				if ( g_pGameRules->IsSpawnPointValid( pNewSpot, this ) )
				{
					if ( pNewSpot->GetAbsOrigin() == vec3_origin )
					{
						continue;
					}
					else if ( pNewSpot->GetTeamNumber() == iTeamNum )
					{
						Msg( "Choosing a generic spawn point of team: %d\n", iTeamNum );
						pSpot = pNewSpot;
						return true;
					}
				}
			}

			pNewSpot = gEntList.FindEntityByClassname( pNewSpot, GenericSpawnPointName );

		} while ( pNewSpot != pFirstSpot );
	}


	if ( iTeamNum == SDK_TEAM_BLUE )
	{
		for ( int i = 0; i < ARRAYSIZE( g_BlueSpawnPoints ); i++ )
		{
			const char *SpawnPoint = g_BlueSpawnPoints[ i ];
			if ( SelectSpawnSpot( SpawnPoint, pSpot ) )
			{
				Msg( "Choosing a blue spawn point\n" );
				return true;
			}
		}
	}
	else if ( iTeamNum == SDK_TEAM_RED )
	{
		for ( int i = 0; i < ARRAYSIZE( g_RedSpawnPoints ); i++ )
		{
			const char *SpawnPoint = g_RedSpawnPoints[ i ];
			if ( SelectSpawnSpot( SpawnPoint, pSpot ) )
			{
				Msg( "Choosing a red spawn point\n" );
				return true;
			}
		}
	}

	return false;
}
#ifdef MOD_SF132
CItemRegister *CModPlayer::GetItemRegister()
{
	return &m_ItemRegister;
}
#endif // MOD_SF132

CModPlayer *ToModPlayer( CBaseEntity *pEntity )
{
	return dynamic_cast< CModPlayer * > ( pEntity );
}

#endif // MOD_ALPHA_PLAYER