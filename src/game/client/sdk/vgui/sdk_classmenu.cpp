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
#include <vgui/IVGUI.h>
#include <vgui_controls/Panel.h>
#include "cdll_util.h"
#include <game/client/iviewport.h>
#include "sdk_backgroundpanel.h"
#include "sdk_gamerules.h"
#include "c_sdk_player.h"
#include "c_sdk_team.h"

#include "sdk_classmenu.h"

#include "IGameUIFuncs.h" // for key bindings
#include "ClientGlobals.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

#if defined ( SDK_USE_PLAYERCLASSES )

ConVar _cl_classmenuopen( "_cl_classmenuopen", "0", FCVAR_CLIENTCMD_CAN_EXECUTE, "internal cvar used to tell server when class menu is open" );
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Panel *CSDKClassInfoPanel::CreateControlByName( const char *controlName )
{
	if ( !Q_stricmp( "CIconPanel", controlName ) )
	{
		return new CIconPanel(this, "icon_panel");
	}
	else
	{
		return BaseClass::CreateControlByName( controlName );
	}
}

void CSDKClassInfoPanel::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

CSDKClassMenu::CSDKClassMenu(IViewPort *pViewPort) : CClassMenu( pViewPort )
{
	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( "resource/GameScheme.res", "GameScheme" );
	SetScheme( scheme );

	m_mouseoverButtons.RemoveAll();
	m_iClassMenuKey = BUTTON_CODE_INVALID;
	m_pInitialButton = NULL;

	m_pClassInfoPanel = new CSDKClassInfoPanel( this, "ClassInfoPanel" );
	
	vgui::ivgui()->AddTickSignal( GetVPanel() );

	m_iActivePlayerClass = -1;
	m_iLastPlayerClassCount = -1;
}
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CSDKClassMenu::CSDKClassMenu(IViewPort *pViewPort, const char *panelName, const char *schemename) : CClassMenu(pViewPort, panelName)
{
	if ( schemename )
	{
		char schemefile[ MAX_PATH + 1 ];
		schemefile[0] = '\0';
		Q_snprintf( schemefile, sizeof( schemefile ), "resource/%s.res", schemename );
		vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( schemefile, schemename );
		SetScheme( scheme );
	}
	else
	{
		vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( "resource/GameScheme.res", "GameScheme" );
		SetScheme( scheme );
	}

	m_mouseoverButtons.RemoveAll();
	m_iClassMenuKey = BUTTON_CODE_INVALID;
	m_pInitialButton = NULL;

	m_pClassInfoPanel = new CSDKClassInfoPanel( this, "ClassInfoPanel" );
	
	vgui::ivgui()->AddTickSignal( GetVPanel() );

	m_bNeedsUpdate = false;

	m_iActivePlayerClass = -1;
	m_iLastPlayerClassCount = -1;
}
//Destructor
CSDKClassMenu::~CSDKClassMenu()
{
}

void CSDKClassMenu::ShowPanel( bool bShow )
{
	if ( bShow )
	{
		engine->CheckPoint( "ClassMenu" );

		m_iClassMenuKey = gameuifuncs->GetButtonCodeForBind( "lf_hud_classmenu" );
	}

	int parentW, parentH;
	GetSize( parentW, parentH );

	BaseClass::ShowPanel( bShow );
}

void CSDKClassMenu::OnKeyCodePressed( KeyCode code )
{
	if ( m_iClassMenuKey != BUTTON_CODE_INVALID && m_iClassMenuKey == code )
	{
		ShowPanel( false );
	}
	else
	{
		if ( code == KEY_0 )
		{
			OnCommand( "vguicancel" );
		}
		else if ( code >= KEY_1 && code <= KEY_9 )
		{
			// Shift keys over to zero offset :/
			// HACK HACK HACK HACK
			int KeyIndex = code - KEY_1;
			char changeclass [128] = {0};
			Q_snprintf( changeclass, sizeof( changeclass ), "cls_%s", GameRules()->GetClassNames( LocalTeamNumber() )[KeyIndex] );
			
			OnCommand( changeclass );

			ShowPanel( false );
		}

		BaseClass::OnKeyCodePressed( code );
	}
}

void CSDKClassMenu::MoveToCenterOfScreen()
{
	int wx, wy, ww, wt;
	surface()->GetWorkspaceBounds(wx, wy, ww, wt);
	SetPos((ww - GetWide()) / 2, (wt - GetTall()) / 2);
}

void CSDKClassMenu::LevelInit()
{
	m_bNeedsUpdate = true;
}

void CSDKClassMenu::Update()
{
	C_SDKPlayer *pPlayer = C_SDKPlayer::GetLocalSDKPlayer();
	
	if ( !pPlayer )
	{
		return;
	}

	int LocalTeam = GetTeamNumber();

	if ( LocalTeam == SDK_TEAM_BLUE )
	{
		LoadControlSettings( SDKGameRules()->GetClassmenuBlue() );
	}
	else if ( LocalTeam == SDK_TEAM_RED )
	{
		LoadControlSettings( SDKGameRules()->GetClassmenuRed() );
	}

	InvalidateLayout( true, true );
	
	if ( pPlayer && pPlayer->m_Shared.DesiredPlayerClass() == PLAYERCLASS_UNDEFINED )
	{
		SetVisibleButton( "CancelButton", false );
	}
	else
	{
		SetVisibleButton( "CancelButton", true ); 
	}

	MoveToCenterOfScreen();

	m_bNeedsUpdate = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Panel *CSDKClassMenu::CreateControlByName( const char *controlName )
{
	if ( !Q_stricmp( "SDKButton", controlName ) )
	{
		MouseOverButton<CSDKClassInfoPanel> *newButton = new MouseOverButton<CSDKClassInfoPanel>( this, NULL, m_pClassInfoPanel );

		if( !m_pInitialButton )
		{
			m_pInitialButton = newButton;
		}

		return newButton;
	}
	else if ( !Q_stricmp( "CIconPanel", controlName ) )
	{
		return new CIconPanel(this, "icon_panel");
	}
	else
	{
		return BaseClass::CreateControlByName( controlName );
	}
}

//-----------------------------------------------------------------------------
// Catch the mouseover event and set the active class
//-----------------------------------------------------------------------------
void CSDKClassMenu::OnShowPage( const char *pagename )
{
	// change which class we are counting based on class name

	// turn the button name into a classname

	char buf[64];

	Q_snprintf( buf, sizeof(buf), "cls_%s", pagename );

	C_SDKTeam *pTeam = dynamic_cast<C_SDKTeam *>( GetGlobalTeam(GetTeamNumber()) );

	if( !pTeam )
		return;

	// Pull the index of this class via IsClassOnTeam
	if ( !pTeam->IsClassOnTeam( buf, m_iActivePlayerClass ) )
	{
		Assert( !"bad class name on class button" );
	}
}

//-----------------------------------------------------------------------------
// Do things that should be done often, eg number of players in the 
// selected class
//-----------------------------------------------------------------------------
void CSDKClassMenu::OnTick( void )
{
	//When a player changes teams, their class and team values don't get here 
	//necessarily before the command to update the class menu. This leads to the cancel button 
	//being visible and people cancelling before they have a class. check for class == PLAYERCLASS_UNASSIGNED and if so
	//hide the cancel button

	if ( !IsVisible() )
		return;

	C_SDKPlayer *pPlayer = C_SDKPlayer::GetLocalSDKPlayer();

	if( pPlayer && pPlayer->m_Shared.PlayerClass() == PLAYERCLASS_UNDEFINED )
	{
		SetVisibleButton("CancelButton", false);
	}

	BaseClass::OnTick();
}

void CSDKClassMenu::SetVisible( bool state )
{
	BaseClass::SetVisible( state );

	if ( state )
	{
		engine->ServerCmd( "menuopen" );			// to the server
		engine->ClientCmd( "_cl_classmenuopen 1" );	// for other panels
	}
	else
	{
		engine->ServerCmd( "menuclosed" );	
		engine->ClientCmd( "_cl_classmenuopen 0" );
	}
}
//-----------------------------------------------------------------------------
// Purpose: Paint background with rounded corners
//-----------------------------------------------------------------------------
void CSDKClassMenu::PaintBackground()
{
	int wide, tall;
	GetSize( wide, tall );

	DrawBackground( m_bgColor, 0, 0, wide, tall );
}

//-----------------------------------------------------------------------------
// Purpose: Paint border with rounded corners
//-----------------------------------------------------------------------------
void CSDKClassMenu::PaintBorder()
{
	int wide, tall;
	GetSize( wide, tall );

	DrawBorder( m_borderColor, wide, tall );
}

//-----------------------------------------------------------------------------
// Purpose: Apply scheme settings
//-----------------------------------------------------------------------------
void CSDKClassMenu::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_bgColor = GetSchemeColor("Lum.Game.Background", GetBgColor(), pScheme);
	m_borderColor = pScheme->GetColor( "Lum.GameBorder", Color( 0, 0, 0, 0 ) );

	const int kGameBorderAlpha = 170;
	m_borderColor.SetColor( m_borderColor.r (), m_borderColor.g (), m_borderColor.b (), kGameBorderAlpha );

	SetBgColor( m_bgColor );
	SetBorder (pScheme->GetBorder ("FrameBorder"));

	DisableFadeEffect(); //Tony; shut off the fade effect because we're using sourcesceheme.
}

#endif // SDK_USE_PLAYERCLASSES
