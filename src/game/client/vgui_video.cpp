//====== Copyright © 1996-2007, Valve Corporation, All rights reserved. =======
//
// Purpose: VGUI panel which can play back video, in-engine
//
//=============================================================================

#include "cbase.h"
#include <vgui/IVGui.h>
#include "vgui/IInput.h"
#include <vgui/ISurface.h>
#include "ienginevgui.h"
#include "iclientmode.h"
#include "vgui_video.h"
#include "engine/ienginesound.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


VideoPanel::VideoPanel( unsigned int nXPos, unsigned int nYPos, unsigned int nHeight, unsigned int nWidth, vgui::VPANEL pParent ) : 
	BaseClass( NULL, "VideoPanel" ),
	m_BIKHandle( BIKHANDLE_INVALID ),
	m_nPlaybackWidth( 0 ),
	m_nPlaybackHeight( 0 )
{
	
	if (pParent == NULL)
	{
		SetParent( enginevgui->GetPanel( PANEL_GAMEUIDLL ) );
	}
	else
	{
		SetParent( pParent );
	}
	
	SetVisible( false );

	m_czLastFile[0] = '\0';
	m_bPlaying = false;
	m_bRepeat = false;

	m_bBlackBackground = true;

	SetKeyBoardInputEnabled( true );
	SetMouseInputEnabled( false );

	SetProportional( false );
	SetVisible( true );
	SetPaintBackgroundEnabled( false );
	SetPaintBorderEnabled( false );
	SetAutoDelete( true );
	
	// Set us up
	SetTall( nHeight );
	SetWide( nWidth );
	SetPos( nXPos, nYPos );

	SetScheme(vgui::scheme()->LoadSchemeFromFile( "resource/VideoPanelScheme.res", "VideoPanelScheme"));
	LoadControlSettings("resource/UI/VideoPanel.res");
}

//-----------------------------------------------------------------------------
// Properly shutdown out materials
//-----------------------------------------------------------------------------
VideoPanel::~VideoPanel( void )
{
	SetParent( (vgui::Panel *) NULL );
}

bool VideoPanel::Play()
{
	return Play( m_czLastFile );
}

//-----------------------------------------------------------------------------
// Purpose: Begins playback of a movie
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool VideoPanel::Play( const char *pFilename )
{
	if ( m_bPlaying )
	{
		Stop();
	}

	// Load and create our BINK video
	m_BIKHandle = bik->CreateMaterial( "VideoBIKMaterial", pFilename, "GAME" );
	if ( m_BIKHandle == BIKHANDLE_INVALID )
	{
		m_bPlaying = false;
		return false;
	}

	int nWidth, nHeight;
	bik->GetFrameSize( m_BIKHandle, &nWidth, &nHeight );
	bik->GetTexCoordRange( m_BIKHandle, &m_flU, &m_flV );
	m_pMaterial = bik->GetMaterial( m_BIKHandle );

	float flFrameRatio = ( (float) GetWide() / (float) GetTall() );
	float flVideoRatio = ( (float) nWidth / (float) nHeight );

	if ( flVideoRatio > flFrameRatio )
	{
		m_nPlaybackWidth = GetWide();
		m_nPlaybackHeight = ( GetWide() / flVideoRatio );
	}
	else if ( flVideoRatio < flFrameRatio )
	{
		m_nPlaybackWidth = ( GetTall() * flVideoRatio );
		m_nPlaybackHeight = GetTall();
	}
	else
	{
		m_nPlaybackWidth = GetWide();
		m_nPlaybackHeight = GetTall();
	}

	m_bPlaying = true;
	Q_strncpy( m_czLastFile, pFilename, sizeof( m_czLastFile ) );

	return true;
}

void VideoPanel::Stop()
{
	if ( !m_bPlaying )
	{
		return;
	}

	// Destroy any previously allocated video
	if ( m_BIKHandle != BIKHANDLE_INVALID )
	{
		bik->DestroyMaterial( m_BIKHandle );
		m_BIKHandle = BIKHANDLE_INVALID;
		m_bPlaying = false;
		//m_bRepeat = false;
	}

	if ( vgui::input()->GetAppModalSurface() == GetVPanel() )
	{
		vgui::input()->ReleaseAppModalSurface();
	}

	vgui::surface()->RestrictPaintToSinglePanel( NULL );
}

bool VideoPanel::IsPlaying()
{
	return m_bPlaying;
}

bool VideoPanel::GetRepeat()
{
	return m_bRepeat;
}

void VideoPanel::SetRepeat( bool bRepeat )
{
	m_bRepeat = bRepeat;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void VideoPanel::Activate( void )
{
	MoveToFront();
	RequestFocus();
	SetVisible( true );
	SetEnabled( true );

	vgui::surface()->SetMinimized( GetVPanel(), false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void VideoPanel::DoModal( void )
{
	MakePopup();
	Activate();

	vgui::input()->SetAppModalSurface( GetVPanel() );
	vgui::surface()->RestrictPaintToSinglePanel( GetVPanel() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void VideoPanel::OnClose( void )
{
	m_bRepeat = false;
	if ( m_bPlaying )
	{
		Stop();
	}

	bik->DestroyMaterial( m_BIKHandle );
	m_BIKHandle = BIKHANDLE_INVALID;
	m_bPlaying = false;
	
	BaseClass::OnClose();

	SetVisible( false );
	MarkForDeletion();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void VideoPanel::GetPanelPos( int &xpos, int &ypos )
{
	xpos = ( (float) ( GetWide() - m_nPlaybackWidth ) / 2 );
	ypos = ( (float) ( GetTall() - m_nPlaybackHeight ) / 2 );
}

//-----------------------------------------------------------------------------
// Purpose: Update and draw the frame
//-----------------------------------------------------------------------------
void VideoPanel::Paint( void )
{
	BaseClass::Paint();

	// No video to play, so do nothing
	if ( m_BIKHandle == BIKHANDLE_INVALID || m_bPlaying == false )
	{
		return;
	}

	// Update our frame
	if ( bik->Update( m_BIKHandle ) == false )
	{
		if ( m_bRepeat )
		{
			Play();
		}
		else
		{
			OnClose();
		}
	}

	// Sit in the "center"
	int xpos, ypos;
	GetPanelPos( xpos, ypos );

	// Black out the background (we could omit drawing under the video surface, but this is straight-forward)
	if ( m_bBlackBackground )
	{
		vgui::surface()->DrawSetColor(  0, 0, 0, 255 );
		vgui::surface()->DrawFilledRect( 0, 0, GetWide(), GetTall() );
	}

	// Draw the polys to draw this out
	CMatRenderContextPtr pRenderContext( materials );
	
	pRenderContext->MatrixMode( MATERIAL_VIEW );
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();

	pRenderContext->MatrixMode( MATERIAL_PROJECTION );
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();

	pRenderContext->Bind( m_pMaterial, NULL );

	CMeshBuilder meshBuilder;
	IMesh* pMesh = pRenderContext->GetDynamicMesh( true );
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	float flLeftX = xpos;
	float flRightX = xpos + (m_nPlaybackWidth-1);

	float flTopY = ypos;
	float flBottomY = ypos + (m_nPlaybackHeight-1);

	// Map our UVs to cut out just the portion of the video we're interested in
	float flLeftU = 0.0f;
	float flTopV = 0.0f;

	// We need to subtract off a pixel to make sure we don't bleed
	float flRightU = m_flU - ( 1.0f / (float) m_nPlaybackWidth );
	float flBottomV = m_flV - ( 1.0f / (float) m_nPlaybackHeight );

	// Get the current viewport size
	int vx, vy, vw, vh;
	pRenderContext->GetViewport( vx, vy, vw, vh );

	// map from screen pixel coords to -1..1
	flRightX = FLerp( -1, 1, 0, vw, flRightX );
	flLeftX = FLerp( -1, 1, 0, vw, flLeftX );
	flTopY = FLerp( 1, -1, 0, vh ,flTopY );
	flBottomY = FLerp( 1, -1, 0, vh, flBottomY );

	float alpha = ((float)GetFgColor()[3]/255.0f);

	for ( int corner=0; corner<4; corner++ )
	{
		bool bLeft = (corner==0) || (corner==3);
		meshBuilder.Position3f( (bLeft) ? flLeftX : flRightX, (corner & 2) ? flBottomY : flTopY, 0.0f );
		meshBuilder.Normal3f( 0.0f, 0.0f, 1.0f );
		meshBuilder.TexCoord2f( 0, (bLeft) ? flLeftU : flRightU, (corner & 2) ? flBottomV : flTopV );
		meshBuilder.TangentS3f( 0.0f, 1.0f, 0.0f );
		meshBuilder.TangentT3f( 1.0f, 0.0f, 0.0f );
		meshBuilder.Color4f( 1.0f, 1.0f, 1.0f, alpha );
		meshBuilder.AdvanceVertex();
	}
	
	meshBuilder.End();
	pMesh->Draw();

	pRenderContext->MatrixMode( MATERIAL_VIEW );
	pRenderContext->PopMatrix();

	pRenderContext->MatrixMode( MATERIAL_PROJECTION );
	pRenderContext->PopMatrix();
}
