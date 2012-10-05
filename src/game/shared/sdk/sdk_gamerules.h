//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The TF Game rules object
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#ifndef SDK_GAMERULES_H
#define SDK_GAMERULES_H

#ifdef _WIN32
#pragma once
#endif


#include "teamplay_gamerules.h"
#include "convar.h"
#include "gamevars_shared.h"
#include "weapon_sdkbase.h"

#ifdef CLIENT_DLL
	#include "c_baseplayer.h"
#else
	#include "player.h"
	#include "sdk_player.h"
	#include "utlqueue.h"
	#include "playerclass_info_parse.h"

#endif

#ifdef CLIENT_DLL
	#define CSDKGameRules C_SDKGameRules
	#define CSDKGameRulesProxy C_SDKGameRulesProxy
#endif


class CSDKGameRulesProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS( CSDKGameRulesProxy, CGameRulesProxy );
	DECLARE_NETWORKCLASS();
};

class CSDKViewVectors : public CViewVectors
{
public:
	CSDKViewVectors( 
		Vector vView,
		Vector vHullMin,
		Vector vHullMax,
		Vector vDuckHullMin,
		Vector vDuckHullMax,
		Vector vDuckView,
		Vector vObsHullMin,
		Vector vObsHullMax,
		Vector vDeadViewHeight
		) :
			CViewVectors( 
				vView,
				vHullMin,
				vHullMax,
				vDuckHullMin,
				vDuckHullMax,
				vDuckView,
				vObsHullMin,
				vObsHullMax,
				vDeadViewHeight )
	{
	}
};

class CSDKGameRules : public CTeamplayRules
{
public:
	DECLARE_CLASS( CSDKGameRules, CTeamplayRules );

	virtual bool	ShouldCollide( int collisionGroup0, int collisionGroup1 );

	virtual int		PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual bool	IsTeamplay( void ) 
	{ 
#if defined ( SDK_USE_TEAMS )
		return true;
#else
		return false;	
#endif
	}
	// Get the view vectors for this mod.
	virtual const CViewVectors* GetViewVectors() const;
	virtual const CSDKViewVectors *GetSDKViewVectors() const;
	//Tony; define a default encryption key.
	virtual const unsigned char *GetEncryptionKey( void ) { return (unsigned char *)"h2e2d3d4"; }
	virtual int GetGameMode( void ) { return GAMEMODE_SDKBASE; }
	virtual int GetGameModeMask( void ) { return GAMEMODE_SDKBASE; }

	//Tony; in shared space
#if defined ( SDK_USE_PLAYERCLASSES )
	const char *GetPlayerClassName( int cls, int team );
#endif

	virtual const char *GetClassmenuRed() const;
	virtual const char *GetClassmenuBlue() const;

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.
	float m_flGameStartTime;

#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.

	CSDKGameRules();
	virtual ~CSDKGameRules();
	virtual float FlPlayerFallDamage( CBasePlayer *pPlayer );
	virtual const char *GetGameDescription( void ) { return SDK_GAME_DESCRIPTION; } 
	virtual bool ClientCommand( CBaseEntity *pEdict, const CCommand &args );

	virtual void RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore, CBaseEntity *pEntityIgnore );
	virtual void RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore );
	virtual void RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore, bool bIgnoreWorld );
	virtual bool IsAllowedToSpawn( CBaseEntity *pEntity );
	virtual void Think();

	virtual void RespawnPlayers ();

public:

	void InitTeams( void );

	void CreateStandardEntities( void );

	virtual const char *GetChatPrefix( bool bTeamOnly, CBasePlayer *pPlayer );
	virtual const char *GetChatFormat( bool bTeamOnly, CBasePlayer *pPlayer );

	CBaseEntity *GetPlayerSpawnSpot( CBasePlayer *pPlayer );
	virtual void PlayerSpawn( CBasePlayer *pPlayer );

#if defined ( SDK_USE_PLAYERCLASSES )
	bool IsPlayerClassOnTeam( int cls, int team );
	bool CanPlayerJoinClass( CSDKPlayer *pPlayer, int cls );
	void ChooseRandomClass( CSDKPlayer *pPlayer );
	bool ReachedClassLimit( int team, int cls );
	int CountPlayerClass( int team, int cls );
	int GetClassLimit( int team, int cls );
#endif 
	bool TeamFull( int team_id );
	bool TeamStacked( int iNewTeam, int iCurTeam );
	int SelectDefaultTeam( void );

	virtual void ServerActivate();

	virtual bool IsSourcefortsMap() { return m_bSourcefortsMap; }
	virtual void SetSourcefortsMap( bool sfmap ) { m_bSourcefortsMap = sfmap; }
protected:
	void CheckPlayerPositions( void );

private:
	void CheckLevelInitialized( void );
	bool m_bLevelInitialized;

	Vector2D	m_vecPlayerPositions[MAX_PLAYERS];

#if defined ( SDK_USE_TEAMS )
	int	m_iSpawnPointCount_Blue;	//number of blue spawns on the map
	int	m_iSpawnPointCount_Red;	//number of red spawns on the map
#endif // SDK_USE_TEAMS

public:
	virtual void DeathNotice( CBasePlayer *pVictim, const CTakeDamageInfo &info );
	const char *GetKillingWeaponName( const CTakeDamageInfo &info, CSDKPlayer *pVictim, int *iWeaponID );
	CNetworkVar( float, m_flGameStartTime );
#endif // CLIENT_DLL

	virtual float GetMapRemainingTime( void );
	virtual float GetMapElapsedTime( void );

#ifdef CLIENT_DLL
	float m_flPhaseTimeLeft;
	int m_iCurrentPhaseID;
	// Is this currently a Sourceforts map?
	bool m_bSourcefortsMap;
#else
	CNetworkVar( float, m_flPhaseTimeLeft );
	CNetworkVar( int, m_iCurrentPhaseID );
	CNetworkVar( bool, m_bSourcefortsMap );
#endif // CLIENT_DLL + else
};

//-----------------------------------------------------------------------------
// Gets us at the game rules
//-----------------------------------------------------------------------------

inline CSDKGameRules* SDKGameRules()
{
	return static_cast<CSDKGameRules*>(g_pGameRules);
}


#endif // SDK_GAMERULES_H
