//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//====================================================================================//

#include "cbase.h"
#include "sdk_textwindow.h"
#include <cdll_client_int.h>
#include <KeyValues.h>

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVgui.h>

#include <FileSystem.h>
#include <convar.h>

#include "sdk_backgroundpanel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

CSDKTextWindow::CSDKTextWindow(IViewPort *pViewPort) : CTextWindow( pViewPort )
{
	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( "resource/GameScheme.res", "GameScheme" );
	SetScheme( scheme );
}

void CSDKTextWindow::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	Color bgColor = GetSchemeColor("Lum.Game.Background", GetBgColor(), pScheme);
	Color borderColor = pScheme->GetColor( "Lum.GameBorder", Color( 0, 0, 0, 0 ) );

	const int kGameBorderAlpha = 170;
	borderColor.SetColor( borderColor.r (), borderColor.g (), borderColor.b (), kGameBorderAlpha );

	SetBgColor( bgColor );
	SetBorder( pScheme->GetBorder ("FrameBorder") );

	Reset();
}

CSDKTextWindow::~CSDKTextWindow()
{
}

void CSDKTextWindow::Update( void )
{
	BaseClass::Update();
}
