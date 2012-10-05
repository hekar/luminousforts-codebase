//====== Copyright © 1996-2007, Valve Corporation, All rights reserved. =======
//
// Purpose: VGUI panel which can play back video, in-engine
//
//=============================================================================

#ifndef VGUI_VIDEO_H
#define VGUI_VIDEO_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>
#include <vgui_controls/EditablePanel.h>
#include "avi/ibik.h"


class VideoPanel : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( VideoPanel, vgui::EditablePanel );
public:

	VideoPanel( unsigned int nXPos, unsigned int nYPos, unsigned int nHeight, unsigned int nWidth, vgui::VPANEL pParent = NULL );

	~VideoPanel( void );

	void Activate( void );
	void Paint( void );
	void DoModal( void );
	void OnClose( void );
	void GetPanelPos( int &xpos, int &ypos );

	bool Play();
	bool Play( const char *pFilename );
	void Stop();

	bool IsPlaying();

	bool GetRepeat();
	void SetRepeat( bool bRepeat );

	void SetBlackBackground( bool bBlack ){ m_bBlackBackground = bBlack; }

protected:
	BIKMaterial_t	m_BIKHandle;
	IMaterial		*m_pMaterial;
	int				m_nPlaybackHeight;			// Calculated to address ratio changes
	int				m_nPlaybackWidth;

	bool			m_bPlaying;
	bool			m_bRepeat;
	char			m_czLastFile [ MAX_PATH + 1 ];

	float			m_flU;	// U,V ranges for video on its sheet
	float			m_flV;

	bool			m_bBlackBackground;
};

#endif // VGUI_VIDEO_H
