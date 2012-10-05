//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "sdk_playerclass_info_parse.h"
#include "weapon_sdkbase.h"
#include <KeyValues.h>

#include "SharedModCvars.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//Tony; due to the nature of the base code.. I must do this !
#define MAX_WEAPON_COUNT 9

FilePlayerClassInfo_t* CreatePlayerClassInfo()
{
	return new CSDKPlayerClassInfo;
}

CSDKPlayerClassInfo::CSDKPlayerClassInfo()
{
	m_iTeam				= TEAM_UNASSIGNED;

	m_szLimitCvar[0]	= '\0';
	m_flRunSpeed		= SDK_DEFAULT_PLAYER_RUNSPEED;
	m_flSprintSpeed		= SDK_DEFAULT_PLAYER_SPRINTSPEED;
	m_flProneSpeed		= SDK_DEFAULT_PLAYER_PRONESPEED;

	m_iHealth			= 0;
	m_iArmor			= 0;
}

void CSDKPlayerClassInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse( pKeyValuesData, szWeaponName );

	m_iTeam= pKeyValuesData->GetInt( "team", TEAM_UNASSIGNED );

	// Figure out what team can have this player class
	m_iTeam = TEAM_UNASSIGNED;

//Tony; don't check for teams unless we're using teams. You could do a free for all, but class / character based game if you wanted.
#ifdef SDK_USE_TEAMS
	const char *pTeam = pKeyValuesData->GetString( "team", NULL );
	if ( pTeam )
	{
		if ( Q_stricmp( pTeam, "BLUE" ) == 0 )
		{
			m_iTeam = SDK_TEAM_BLUE;
		}
		else if ( Q_stricmp( pTeam, "RED" ) == 0 )
		{
			m_iTeam = SDK_TEAM_RED;
		}
		else
		{
			Assert( false );
		}
	}
	else
	{
		Assert( false );
	}
#endif

	for (int i = 1; i <= MAX_WEAPON_COUNT; i++)
	{
		int Length = strlen ("weapon_") + 2;
		char *WeaponKeyName = new char [Length];
		Q_snprintf (WeaponKeyName, Length, "weapon_%d", i);
		const char *pszWeapon = pKeyValuesData->GetString( WeaponKeyName, NULL );
		
		if (!pszWeapon) // At the end of the weaponlist, give up
		{
			break;
		}

		int iWeapon = AliasToWeaponID( pszWeapon );

		m_WeaponVector.AddToTail (iWeapon);
		delete WeaponKeyName;

		int AmmoLength = strlen ("weapon_ammo") + 3;
		char *AmmoKeyName = new char [AmmoLength];
		Q_snprintf (AmmoKeyName, AmmoLength, "weapon_%d_ammo", i);
		int iAmmoCount = pKeyValuesData->GetInt( AmmoKeyName, 0 );

		m_AmmoVector.AddToTail (iAmmoCount);
		delete AmmoKeyName;

		m_iWeaponCount = i;
	}

	Q_strncpy( m_szLimitCvar, pKeyValuesData->GetString( "limitcvar", "!! Missing limit cvar on Player Class" ), sizeof(m_szLimitCvar) );

	Assert( Q_strlen( m_szLimitCvar ) > 0 && "Every class must specify a limitcvar" );

	// HUD player status health images (when the player is hurt)
	Q_strncpy( m_szClassImage, pKeyValuesData->GetString( "classimage", "white" ), sizeof( m_szClassImage ) );
	Q_strncpy( m_szClassImageBG, pKeyValuesData->GetString( "classimagebg", "white" ), sizeof( m_szClassImageBG ) );

	m_iHealth		= pKeyValuesData->GetFloat( "Health", 100 );
	m_flRunSpeed		= pKeyValuesData->GetFloat( "RunSpeed", SDK_DEFAULT_PLAYER_RUNSPEED );
	m_flSprintSpeed		= pKeyValuesData->GetFloat( "SprintSpeed", SDK_DEFAULT_PLAYER_SPRINTSPEED );
	m_flProneSpeed		= pKeyValuesData->GetFloat( "ProneSpeed", SDK_DEFAULT_PLAYER_PRONESPEED );
	m_flStaminaDrainRate = pKeyValuesData->GetFloat( "StaminaDrainRate", lf_combat_default_drainrate.GetFloat() );
	m_flStaminaRestoreRate = pKeyValuesData->GetFloat( "StaminaRestoreRate", lf_combat_default_restorerate.GetFloat() );

	m_iArmor			= pKeyValuesData->GetInt( "armor", 0 );

}
