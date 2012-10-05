//======== Copyright © 1996-2008, Valve Corporation, All rights reserved. =========//
//
// Purpose: 
//
// $NoKeywords: $
//=================================================================================//

#ifndef SDK_BACKHROUNDPANEL_H
#define SDK_BACKHROUNDPANEL_H

#include <vgui_controls/Frame.h>
#include <vgui_controls/EditablePanel.h>
#include "HudH_Utils.h"

void DrawRoundedBorder( Color borderColor, int wide, int tall );
void DrawRoundedBackground( Color bgColor, int wide, int tall );

void DrawGradient( Color firstColor, Color secondColor, int x, int y, int wide, int tall, float alpha0, float alpha1, bool horizontal = false );

//-----------------------------------------------------------------------------
#endif // SDK_BACKHROUNDPANEL_H
