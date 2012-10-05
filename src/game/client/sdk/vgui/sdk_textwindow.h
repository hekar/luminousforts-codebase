//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef CSDK_TEXTWINDOW_H
#define CSDK_TEXTWINDOW_H
#ifdef _WIN32
#pragma once
#endif

#include <vguitextwindow.h>

//-----------------------------------------------------------------------------
// Purpose: Game ScoreBoard
//-----------------------------------------------------------------------------
class CSDKTextWindow : public CTextWindow
{
private:
	DECLARE_CLASS_SIMPLE(CSDKTextWindow, CTextWindow);
	
public:
	CSDKTextWindow(IViewPort *pViewPort);
	~CSDKTextWindow();

	virtual void Update();

protected:
	// vgui overrides for rounded corner background
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

};


#endif // CSDK_TEXTWINDOW_H
