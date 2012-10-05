//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef SDK_PLAYERCLASS_INFO_PARSE_H
#define SDK_PLAYERCLASS_INFO_PARSE_H

#ifdef _WIN32
#pragma once
#endif

#include "playerclass_info_parse.h"
#include "networkvar.h"

//--------------------------------------------------------------------------------------------------------
class CSDKPlayerClassInfo : public FilePlayerClassInfo_t
{
public:
	DECLARE_CLASS_GAMEROOT( CSDKPlayerClassInfo, FilePlayerClassInfo_t );
	
	CSDKPlayerClassInfo();
	
	virtual void Parse( ::KeyValues *pKeyValuesData, const char *szWeaponName );

	int m_iTeam;		//which team. 2 == team 1, 3 == team 2

	char m_szLimitCvar[64];	//which cvar controls the class limit for this class

	char m_szClassImage[SDK_PLAYERCLASS_IMAGE_LENGTH];
	char m_szClassImageBG[SDK_PLAYERCLASS_IMAGE_LENGTH];

	float m_flRunSpeed;
	float m_flSprintSpeed;
	float m_flProneSpeed;
	float m_flStaminaDrainRate;
	float m_flStaminaRestoreRate;

	int m_iHealth;
	int m_iArmor;

	int m_iWeaponCount;
	CUtlVector <int> m_WeaponVector;
	CUtlVector <int> m_AmmoVector; // Primary Ammo, add one for secondary when needed
};

#endif // SDK_PLAYERCLASS_INFO_PARSE_H
