//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef SDK_SHAREDDEFS_H
#define SDK_SHAREDDEFS_H

#ifdef _WIN32
#pragma once
#endif

#define SDK_GAME_DESCRIPTION	"LuminousForts 0.1"

#define SDK_USE_TEAMS
#define SDK_USE_PLAYERCLASSES
#define SDK_USE_STAMINA
#define SDK_USE_SPRINTING
#define SDK_SHOOT_WHILE_SPRINTING
#define SDK_SHOOT_ON_LADDERS
#define SDK_SHOOT_WHILE_JUMPING

// Can your players go prone?
//#define SDK_USE_PRONE



#define SDK_TEAM_SPECTATOR TEAM_SPECTATOR
enum sdkteams_e
{
	SDK_TEAM_BLUE = LAST_SHARED_TEAM+1,
	SDK_TEAM_RED,
};

#define INITIAL_SPRINT_STAMINA_PENALTY 0
#define LOW_STAMINA_THRESHOLD	35

#define SDK_NUM_PLAYERCLASSES GameRules()->GetClassCount()
#define SDK_PLAYERCLASS_IMAGE_LENGTH 64

#define PLAYERCLASS_RANDOM		-2
#define PLAYERCLASS_UNDEFINED	-1

//Tony; using teams with classes, so make sure the team class panel names are defined.
#define PANEL_CLASS_BLUE		"class_blue"
#define PANEL_CLASS_RED			"class_red"

#define pszTeamBlueClasses GameRules()->GetClassNames( SDK_TEAM_BLUE )
#define pszTeamRedClasses GameRules()->GetClassNames( SDK_TEAM_RED )
//extern const char *pszTeamBlueClasses[];
//extern const char *pszTeamRedClasses[];

//Tony; We need to precache all possible player models that we're going to use
extern const char *pszPossiblePlayerModels[];

extern const char *pszTeamNames[];

//Tony; these defines handle the default speeds for all of these - all are listed regardless of which option is enabled.
#define SDK_DEFAULT_PLAYER_RUNSPEED			340
#define SDK_DEFAULT_PLAYER_SPRINTSPEED		440
#define SDK_DEFAULT_PLAYER_PRONESPEED		100

//--------------------------------------------------------------------------------------------------------
//
// Weapon IDs for all SDK Game weapons
//
typedef enum
{
	WEAPON_NONE = 0,

	SDK_WEAPON_NONE = WEAPON_NONE,
	SDK_WEAPON_MP5,
	SDK_WEAPON_RPG,
	SDK_WEAPON_SHOTGUN,
	SDK_WEAPON_GRENADE,
	SDK_WEAPON_PISTOL,
	SDK_WEAPON_CROWBAR,
	SDK_WEAPON_PHYSCANNON,
	SDK_WEAPON_SMG1,
	SDK_WEAPON_AR2,
	SDK_WEAPON_HL2_PISTOL,
	SDK_WEAPON_HL2_SHOTGUN,
	SDK_WEAPON_CROSSBOW,
	SDK_WEAPON_FRAG,
	SDK_WEAPON_357,
	SDK_WEAPON_SLAM,
	LF_WEAPON_BUILDCANNON,
	LF_WEAPON_COMBATCANNON,
	LF_WEAPON_MEDIUMSHOTGUN,
	LF_WEAPON_RAILGUN,
	LF_WEAPON_AIRGUN,
	LF_WEAPON_AIRBLASTER,
	LF_WEAPON_GRENADE_LAUNCHER,
	LF_WEAPON_CONC,
	LF_WEAPON_BEACON,
	LF_WEAPON_ORBIT_CANNON,
	SF132_WEAPON_FREEZER,
	SF132_WEAPON_PHYSCANNON,
	LF_WEAPON_QUICKPISTOL,
	LF_WEAPON_SUBMACHINEGUN,
	LF_WEAPON_ENGINEERFRAG,
	LF_WEAPON_IMPACTGL,
	LF_WEAPON_MEDIKIT,
	LF_WEAPON_BUILDTOOL,
	
	WEAPON_MAX,		// number of weapons weapon index
} SDKWeaponID;

typedef enum
{
	FM_AUTOMATIC = 0,
	FM_SEMIAUTOMATIC,
	FM_BURST,

} SDK_Weapon_Firemodes;

const char *WeaponIDToAlias( int id );
int AliasToWeaponID( const char *alias );


// The various states the player can be in during the join game process.
enum SDKPlayerState
{
	// Happily running around in the game.
	// You can't move though if CSGameRules()->IsFreezePeriod() returns true.
	// This state can jump to a bunch of other states like STATE_PICKINGCLASS or STATE_DEATH_ANIM.
	STATE_ACTIVE=0,
	
	// This is the state you're in when you first enter the server.
	// It's switching between intro cameras every few seconds, and there's a level info 
	// screen up.
	STATE_WELCOME,			// Show the level intro screen.
	
	// During these states, you can either be a new player waiting to join, or
	// you can be a live player in the game who wants to change teams.
	// Either way, you can't move while choosing team or class (or while any menu is up).
	STATE_PICKINGTEAM,			// Choosing team.
	STATE_PICKINGCLASS,			// Choosing class.
	
	STATE_DEATH_ANIM,			// Playing death anim, waiting for that to finish.
	STATE_OBSERVER_MODE,		// Noclipping around, watching players, etc.

	NUM_PLAYER_STATES
};

#define SDK_PLAYER_DEATH_TIME	3.0f	//Minimum Time before respawning

// Special Damage types
enum
{
	SDK_DMG_CUSTOM_NONE = 0,
	SDK_DMG_CUSTOM_SUICIDE,
};

// Player avoidance
#define PUSHAWAY_THINK_INTERVAL		(1.0f / 20.0f)

#endif // SDK_SHAREDDEFS_H
