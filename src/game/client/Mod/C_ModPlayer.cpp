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
#ifdef MOD_ALPHA_PLAYER
#include "c_sdk_player.h"
#include "C_ModPlayer.h"
#include "CModPlayerShared.h"

// CModPlayerShared Data Tables
//=============================
// specific to the local player ( ideally should not be in CModPlayerShared! )
BEGIN_RECV_TABLE_NOBASE( CModPlayerShared, DT_ModSharedLocalPlayerExclusive )
END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE( CModPlayerShared, DT_ModPlayerShared )
END_RECV_TABLE()

// main table
IMPLEMENT_CLIENTCLASS_DT( C_ModPlayer, DT_ModPlayer, CModPlayer )
END_RECV_TABLE()

// ------------------------------------------------------------------------------------------ //
// Prediction tables.
// ------------------------------------------------------------------------------------------ //
BEGIN_PREDICTION_DATA_NO_BASE( CModPlayerShared )
END_PREDICTION_DATA()

BEGIN_PREDICTION_DATA( C_ModPlayer )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( modplayer, C_ModPlayer );

C_ModPlayer::C_ModPlayer()
{
}

C_ModPlayer::~C_ModPlayer()
{
}

C_ModPlayer* C_ModPlayer::GetLocalModPlayer()
{
	return ToModPlayer( C_BasePlayer::GetLocalPlayer() );
}

C_ModPlayer *ToModPlayer( C_BasePlayer *pPlayer )
{
	return dynamic_cast< C_ModPlayer * > ( pPlayer );
}

extern ConVar hud_classautokill;

class CPlayerInitGameSystem : public CAutoGameSystem
{
public:
	CPlayerInitGameSystem() :
	  CAutoGameSystem( "PlayerInitGameSystem" )
	{
	}

	// When map loading is nearly finished
	void LevelInitPostEntity()
	{
		hud_classautokill.SetValue( 0 );
	}
};

static CPlayerInitGameSystem g_PlayerInitGameSystem;

#endif // MOD_ALPHA_PLAYER