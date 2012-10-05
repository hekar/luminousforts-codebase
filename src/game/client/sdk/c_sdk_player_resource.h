//========= Copyright � 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: SDK C_PlayerResource
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_SDK_PLAYER_RESOURCE_H
#define C_SDK_PLAYER_RESOURCE_H
#ifdef _WIN32
#pragma once
#endif

#include "c_playerresource.h"

class C_SDK_PlayerResource : public C_PlayerResource
{
	DECLARE_CLASS( C_SDK_PlayerResource, C_PlayerResource );
public:
	DECLARE_CLIENTCLASS();

					C_SDK_PlayerResource();
	virtual			~C_SDK_PlayerResource();

	int GetPlayerClass( int iIndex );
	
protected:
	int	m_iPlayerClass[MAX_PLAYERS+1];
};
C_SDK_PlayerResource * SDKGameResources( void );

#endif // C_SDK_PLAYERRESOURCE_H
