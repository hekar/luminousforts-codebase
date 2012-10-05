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


#ifndef GUI_LISTENSERVER_H
#define GUI_LISTENSERVER_H

#ifdef _WIN32
#pragma once
#endif

#include "BaseGUI.h"
#include <vgui_controls/PropertyDialog.h>
#include "FileSystem.h"

class CGUIListenServer : public vgui::PropertyDialog
{
private:
	DECLARE_CLASS_SIMPLE( CGUIListenServer, vgui::PropertyDialog );

public:
	CGUIListenServer( vgui::VPANEL parent );
	~CGUIListenServer();

	void ShowPanel( bool bShow );

	void SetVisible( bool state ){ BaseClass::SetVisible( state ); }

	// both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
  	bool IsVisible() { return BaseClass::IsVisible(); }
	void SetParent( vgui::VPANEL parent ) { BaseClass::SetParent( parent ); }

protected:
	ProtectedMethod void ApplySchemeSettings( vgui::IScheme *pScheme );
};

DEFINE_GUI_INTERFACE( CGUIListenServer, ListenServerInterface, listenserver );

#endif // GUI_LISTENSERVER_H