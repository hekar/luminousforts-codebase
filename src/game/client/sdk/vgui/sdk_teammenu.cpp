//======== Copyright © 1996-2008, Valve Corporation, All rights reserved. =========//
//
// Purpose: 
//
// $NoKeywords: $
//=================================================================================//

#include "cbase.h"
#include <stdio.h>

#include <cdll_client_int.h>

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/ImageList.h>
#include <FileSystem.h>

#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Panel.h>

#include "cdll_util.h"

#include <game/client/iviewport.h>

#include "sdk_backgroundpanel.h"
#include "sdk_classmenu.h"
#include "sdk_teammenu.h"
#include "sdk_gamerules.h"
#include "c_sdk_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

#if defined ( SDK_USE_TEAMS )
CSDKTeamMenu::CSDKTeamMenu(IViewPort *pViewPort) : CTeamMenu( pViewPort )
{
	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( "resource/GameScheme.res", "GameScheme" );
	SetScheme( scheme );
}

CSDKTeamMenu::~CSDKTeamMenu()
{
}
void CSDKTeamMenu::MoveToCenterOfScreen()
{
	int wx, wy, ww, wt;
	surface()->GetWorkspaceBounds(wx, wy, ww, wt);
	SetPos((ww - GetWide()) / 2, (wt - GetTall()) / 2);
}

void CSDKTeamMenu::OnKeyCodePressed( KeyCode code )
{
	if ( IsVisible() )
	{
		if ( code == KEY_0 )
		{
			OnCommand( "vguicancel" );
		}
		else if ( code == KEY_1 )
		{
			// Join blue
			OnCommand( "jointeam 2" );
		}
		else if ( code == KEY_2 )
		{
			// Join red
			OnCommand( "jointeam 3" );
		}
		else if ( code == KEY_3 )
		{
			// Auto assign
			OnCommand( "jointeam 0" );
		}
		else if ( code == KEY_4 )
		{
			// Spectator
			OnCommand( "jointeam 1" );
		}
	}
}
//-----------------------------------------------------------------------------
// Purpose: called to update the menu with new information
//-----------------------------------------------------------------------------
void CSDKTeamMenu::Update( void )
{
	BaseClass::Update();

	const ConVar *allowspecs =  cvar->FindVar( "mp_allowspectators" );

	C_SDKPlayer *pPlayer = C_SDKPlayer::GetLocalSDKPlayer();
	
	if ( !pPlayer || !SDKGameRules() )
		return;

	if ( allowspecs && allowspecs->GetBool() )
	{
		SetVisibleButton("specbutton", true);
	}
	else
	{
		SetVisibleButton("specbutton", false );
	}

	MoveToCenterOfScreen();
}

void CSDKTeamMenu::SetVisible(bool state)
{
	BaseClass::SetVisible( state );

	if ( state )
	{
		Panel *pAutoButton = FindChildByName( "autobutton" );
		if ( pAutoButton )
		{
			pAutoButton->RequestFocus();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: When a team button is pressed it triggers this function to 
//			cause the player to join a team
//-----------------------------------------------------------------------------
void CSDKTeamMenu::OnCommand( const char *command )
{
	if ( Q_stricmp( command, "vguicancel" ) )
	{
		C_SDKPlayer *pPlayer = (C_SDKPlayer *)C_SDKPlayer::GetLocalPlayer();
		
		if ( !Q_stricmp( command, "jointeam 2" ) )
		{
			pPlayer->SetLocalTeamNumber( SDK_TEAM_BLUE );
		}
		else if ( !Q_stricmp( command, "jointeam 3" ) )
		{
			pPlayer->SetLocalTeamNumber( SDK_TEAM_RED );
		}

		engine->ClientCmd( command );
	}

	CSDKClassMenu *classmenu = dynamic_cast< CSDKClassMenu * > ( gViewPortInterface->FindPanelByName( PANEL_CLASS_BLUE ) );
	if ( classmenu )
	{
		classmenu->Update();
	}
	else
	{
		classmenu = dynamic_cast< CSDKClassMenu * > ( gViewPortInterface->FindPanelByName( PANEL_CLASS_RED ) );
		if ( classmenu )
		{
			classmenu->Update();
		}
	}
	
	BaseClass::OnCommand( command );

	gViewPortInterface->ShowBackGround( false );
	OnClose();
}

//-----------------------------------------------------------------------------
// Purpose: Sets the visibility of a button by name
//-----------------------------------------------------------------------------
void CSDKTeamMenu::SetVisibleButton(const char *textEntryName, bool state)
{
	Button *entry = dynamic_cast<Button *>( FindChildByName( textEntryName ) );
	if ( entry )
	{
		entry->SetVisible( state );
	}
}

void CSDKTeamMenu::PaintBackground()
{
	int wide, tall;
	GetSize( wide, tall );

	DrawBackground( m_bgColor, 0, 0, wide, tall );
}

void CSDKTeamMenu::PaintBorder()
{
	int wide, tall;
	GetSize( wide, tall );

	DrawBorder( m_borderColor, wide, tall );
}

void CSDKTeamMenu::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_bgColor = GetSchemeColor("Lum.Game.Background", GetBgColor(), pScheme);
	m_borderColor = pScheme->GetColor( "Lum.GameBorder", Color( 0, 0, 0, 0 ) );

	const int kGameBorderAlpha = 170;
	m_borderColor.SetColor( m_borderColor.r (), m_borderColor.g (), m_borderColor.b (), kGameBorderAlpha );

	SetBgColor( m_bgColor );
	SetBorder( pScheme->GetBorder( "BaseBorder") );

	DisableFadeEffect(); //Tony; shut off the fade effect because we're using sourcesceheme.
}
#endif // SDK_USE_TEAMS
