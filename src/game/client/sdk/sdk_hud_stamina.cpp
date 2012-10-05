//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "sdk_hud_stamina.h"
#include "hud_macros.h"
#include "c_sdk_player.h"
#include "iclientmode.h"
#include <vgui_controls/AnimationController.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>

#include "ClassicGameRules.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if defined ( SDK_USE_STAMINA ) || defined ( SDK_USE_SPRINTING )

DECLARE_HUDELEMENT( CHudStamina );

#define STAMINA_INIT -1

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudStamina::CHudStamina( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudSuitPower" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	vgui::HScheme scheme = vgui::scheme()->GetScheme( "ClientScheme" );
	m_hTextFont = vgui::scheme()->GetIScheme( scheme )->GetFont( "SFHUDLarge" );

	SetHiddenBits( HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT | HIDEHUD_BUILDPHASE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudStamina::Init( void )
{
	m_flStamina = STAMINA_INIT;
	m_flStaminaLow = -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudStamina::Reset( void )
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHudStamina::ShouldDraw()
{
	C_SDKPlayer *pPlayer = C_SDKPlayer::GetLocalSDKPlayer();
	if ( !pPlayer )
		return false;
	if ( pPlayer->State_Get() != STATE_ACTIVE )
		return false;
	if ( g_pGameRules->GetGameMode() == GAMEMODE_CLASSIC && ClassicGameRules()->GetCurrentPhaseID() == PHASE_BUILD )
	{
		if ( ClassicGameRules()->GetCurrentPhaseID() == PHASE_BUILD )
		{
			return false;
		}
	}

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudStamina::OnThink( void )
{
	float flCurrentStamina = 0;
	C_SDKPlayer *pPlayer = C_SDKPlayer::GetLocalSDKPlayer();
	if ( !pPlayer )
		return;

	flCurrentStamina = pPlayer->m_Shared.GetStamina();

	// Only update if we've changed stamina
	if ( flCurrentStamina == m_flStamina )
		return;

	if ( flCurrentStamina >= 100.0f && m_flStamina < 100.0f )
	{
		// we've reached max stamina
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("SuitStaminaMax");
	}
	else if ( flCurrentStamina < 100.0f && (m_flStamina >= 100.0f || m_flStamina == STAMINA_INIT) )
	{
		// we've lost stamina
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("SuitStaminaNotMax");
	}
	m_flStamina = flCurrentStamina;
}

//-----------------------------------------------------------------------------
// Purpose: draws the stamina bar
//-----------------------------------------------------------------------------
void CHudStamina::Paint()
{
	C_SDKPlayer *pPlayer = C_SDKPlayer::GetLocalSDKPlayer();
	if ( !pPlayer )
		return;

	// get bar chunks
	int chunkCount = m_flBarWidth / (m_flBarChunkWidth + m_flBarChunkGap);
	int enabledChunks = (int)((float)chunkCount * (m_flStamina * 1.0f/100.0f) + 0.5f );

	// see if we've changed stamina state
	int lowStamina = 0;
	if (enabledChunks <= (chunkCount / 4))
	{
		lowStamina = 1;
	}
	if (m_flStaminaLow != lowStamina)
	{
		if ( m_flStamina < 100.0f )
		{
			if (lowStamina)
			{
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("SuitStaminaDecreasedBelow25");
			}
			else
			{
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("SuitStaminaIncreasedAbove25");
			}
			m_flStaminaLow = lowStamina;
		}
	}

	if (pPlayer->GetTeamNumber () == SDK_TEAM_RED)
	{
		m_StaminaColor = Color (128, 32, 16, 255);
	}
	else if (pPlayer->GetTeamNumber () == SDK_TEAM_BLUE)
	{
		m_StaminaColor = Color (0, 128, 220, 255);
	}
	// Kohan: New stamina bar code.
	// draw the stamina bar
	surface()->DrawSetColor( m_StaminaColor );
	surface()->DrawFilledRect(m_flBarInsetX, m_flBarInsetY, m_flBarInsetX + m_flStamina / 100.0f * m_flBarWidth, m_flBarInsetY + m_flBarHeight);
	surface()->DrawSetColor( Color( m_StaminaColor[0], m_StaminaColor[1], m_StaminaColor[2], m_iStaminaDisabledAlpha ) );
	surface()->DrawFilledRect(m_flBarInsetX + m_flStamina / 100.0f * m_flBarWidth, m_flBarInsetY, m_flBarInsetX + m_flBarWidth, m_flBarInsetY + m_flBarHeight);

	// draw our name
	surface()->DrawSetTextFont( m_hTextFont );
	surface()->DrawSetTextColor( Color( 255, 255, 255, 255 ) );
	surface()->DrawSetTextPos( text_xpos, text_ypos );
	surface()->DrawPrintText( L"Stamina", sizeof( "Stamina" ) - 1 );
}

#endif // SDK_USE_STAMINA || SDK_USE_SPRINTING

