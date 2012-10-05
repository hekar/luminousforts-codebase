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


#ifndef NEWHTML_H
#define NEWHTML_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/IHTML.h>
#include <vgui_controls/HTML.h>

struct PageHistory_t
{
	char Name[ 128 ];
	char URL[ 512 ];
};

namespace vgui
{

class NewHTML : public vgui::HTML
{
	DECLARE_CLASS_SIMPLE( NewHTML, HTML );
public:
	NewHTML( vgui::Panel *parent, const char *name );
	virtual ~NewHTML();

	virtual void Back();
	virtual void Next();

	virtual int GetBackCount();
	virtual int GetNextCount();

	// Limit number of back pages
	virtual void SetBackLimit( int backlimit );

	virtual void OpenURL(const char *URL, bool force = false);
	virtual void GoTo( const char *URL, bool savehistory = true );
	virtual bool Stop();
	virtual bool Refresh();

	virtual bool OnStartURL(const char *url, const char *target, bool first);
	virtual void OnFinishURL(const char *url);

	virtual void SetVisible( bool state )
	{
		BaseClass::SetVisible( state );
	}

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)
	{
		BaseClass::ApplySchemeSettings( pScheme );
	}

	virtual void ApplySettings( KeyValues *inResourceData )
	{
		BaseClass::ApplySettings( inResourceData );
	}

private:
	PageHistory_t _CurrentPage;
	// A Stack gives an easier interface, but a list gives more power
	CUtlLinkedList< PageHistory_t > _BackHistory;
	CUtlLinkedList< PageHistory_t > _ForwardHistory;

	int _BackLimit;
};

}; // namespace vgui

#endif // NEWHTML_H