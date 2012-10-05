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
#include "sdk_team.h"
#include "CBlockBase.h"

ConVar lf_block_dissolve( "lf_block_dissolve", "0" );

CBlockBase *FindBlock( CBasePlayer *player, int distance )
{
	Vector vecSrc, vecDir, vecEnd;

	vecSrc = player->Weapon_ShootPosition();
	player->EyeVectors( &vecDir );

	vecEnd = vecSrc + vecDir * distance;
	
	// Create the traceline
	trace_t tr;
	UTIL_TraceLine( vecSrc, vecEnd, MASK_SHOT, player, COLLISION_GROUP_NONE, &tr );
	
	CBaseEntity* pEntity = tr.m_pEnt;
	
	CBlockBase *pBlock = dynamic_cast< CBlockBase * > ( pEntity );

	if( !pBlock )
	{
		return NULL;
	}

	return pBlock;
}

trace_t& GetPlayerTraceLine( CBasePlayer *player, int distance )
{
	Vector vecSrc, vecDir, vecEnd;

	vecSrc = player->Weapon_ShootPosition();
	player->EyeVectors( &vecDir );

	vecEnd = vecSrc + vecDir * distance;
	
	// Create the traceline
	trace_t tr;
	UTIL_TraceLine( vecSrc, vecEnd, MASK_SHOT, player, COLLISION_GROUP_NONE, &tr );
	
	return tr;
}

Vector GetPlayerTraceLineEnd( CBasePlayer *player, int distance )
{
	Vector vecSrc, vecDir, vecEnd;

	vecSrc = player->Weapon_ShootPosition();
	player->EyeVectors( &vecDir );

	vecEnd = vecSrc + vecDir * distance;
	
	// Create the traceline
	trace_t tr;
	UTIL_TraceLine( vecSrc, vecEnd, MASK_SHOT, player, COLLISION_GROUP_NONE, &tr );
	
	return vecEnd;
}

CBaseEntity *SpawnBlock( int blockType, int team, const Vector& origin, const QAngle& angles, CBaseEntity *parent )
{
	if ( blockType >= BLOCK_1x2 && blockType < BLOCK_LAST )
	{
		CBaseEntity *pEntity = CBaseEntity::CreateNoSpawn ( g_szBlocks[ blockType ], origin, angles, parent );
		CBlockBase *pBlock = dynamic_cast< CBlockBase * > ( pEntity );
		if ( !pBlock )
		{
			return NULL;
		}

		pBlock->ChangeTeam( team );

		if ( g_pGameRules->IsAllowedToSpawn( pBlock ) )
		{
			CSDKTeam *pTeam = static_cast< CSDKTeam *> ( pBlock->GetTeam() );
			pTeam->AddBlockCount( pBlock->GetBlockWorth() );
			pBlock->Spawn();
		}
		else
		{
			UTIL_Remove( pBlock );
			return NULL;
		}

		return pEntity;
	}
	else
	{
		return NULL;
	}
}

bool DisposeBlock( CBaseEntity *ent )
{
	if ( ent->IsBlock() ) // If the entity is a block
	{
		CBlockBase *pBlock = dynamic_cast< CBlockBase * > ( ent );

		CSDKTeam *pTeam = dynamic_cast< CSDKTeam * > ( pBlock->GetTeam() );
		Assert( pTeam );
		pTeam->AddBlockCount( -pBlock->GetBlockWorth() );

		if ( lf_block_dissolve.GetBool() )
		{
			pBlock->Dissolve( NULL, gpGlobals->curtime, false, ENTITY_DISSOLVE_ELECTRICAL );
		}
		else
		{
			pBlock->Remove();
		}

		return true;
	}
	else
	{
		return false;
	}
}