//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef CSDK_SCOREBOARD_H
#define CSDK_SCOREBOARD_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Label.h>
#include <clientscoreboarddialog.h>

//-----------------------------------------------------------------------------
// Purpose: Game ScoreBoard
//-----------------------------------------------------------------------------
class CSDKScoreboard : public CClientScoreBoardDialog
{
private:
	DECLARE_CLASS_SIMPLE(CSDKScoreboard, CClientScoreBoardDialog);
	
public:
	CSDKScoreboard(IViewPort *pViewPort);
	~CSDKScoreboard();


protected:
	// scoreboard overrides
	virtual void InitScoreboardSections();
	virtual void UpdateTeamInfo();
	virtual void UpdatePlayerInfo();

	// vgui overrides for rounded corner background
	virtual void PaintBackground();
	virtual void PaintBorder();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

private:
	virtual void AddHeader(vgui::SectionedListPanel *pList); // add the start header of the scoreboard
	virtual void AddSection(vgui::SectionedListPanel *pList, int teamType, int teamNumber); // add a new section header for a team

	int GetSectionFromTeamNumber( int teamNumber );

	vgui::Label *m_pSpecLabel;

	Color m_bgColor;
	Color m_borderColor;
};


#endif // CSDK_SCOREBOARD_H
