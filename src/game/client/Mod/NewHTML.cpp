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


/*===============================================================
	Client

	Extended HTML widget with new external interface

	
	Note:
		This code has quite a few issues and isn't bugfree

	Last Updated Jan 21, 2009
===============================================================*/

#include "cbase.h"
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include "NewHTML.h"

#define NEWHTML_BACKLIMIT 10
#define NEWHTML_JAVASCRIPT true

namespace vgui
{

NewHTML::NewHTML( vgui::Panel *parent, const char *name ) :
	BaseClass( parent, name, NEWHTML_JAVASCRIPT )
{
	_CurrentPage.Name[ 0 ] = '\0';
	_CurrentPage.URL[ 0 ] = '\0';

	_BackLimit = NEWHTML_BACKLIMIT;
}

NewHTML::~NewHTML()
{
}

void NewHTML::Back()
{
	if ( ( _BackHistory.Count() + 1 ) > _BackLimit )
	{
		return;
	}

	_ForwardHistory.AddToTail( _CurrentPage );

	int lastpageindex = _BackHistory.Count() - 1;
	GoTo( _BackHistory[ lastpageindex ].URL, false );

	_BackHistory.Remove( lastpageindex );
}

void NewHTML::Next()
{
	GoTo( _ForwardHistory[ 0 ].URL, true );
	_ForwardHistory.Remove( 0 );
}

int NewHTML::GetBackCount()
{
	return _BackHistory.Count();
}

int NewHTML::GetNextCount()
{
	return _ForwardHistory.Count();
}

void NewHTML::SetBackLimit( int backlimit )
{
	_BackLimit = backlimit;
}

void NewHTML::OpenURL(const char *URL, bool force)
{
	BaseClass::OpenURL( URL, force );
}

void NewHTML::GoTo( const char *URL, bool savehistory)
{
	if ( savehistory )
	{
		_BackHistory.AddToTail( _CurrentPage );
	}

	Q_strncpy( _CurrentPage.URL, URL, sizeof( _CurrentPage.URL ) );

	OpenURL( URL, true );
}

bool NewHTML::Stop()
{
	return BaseClass::StopLoading();
}

bool NewHTML::Refresh()
{
	return BaseClass::Refresh();
}

bool NewHTML::OnStartURL(const char *url, const char *target, bool first)
{
	bool loading = BaseClass::OnStartURL( url, target, first );

	if ( loading )
	{
		if (GetVParent())
		{
			KeyValues *msg = new KeyValues("StartLoading");
			msg->SetString( "url", url );
			msg->SetString( "target", target );
			ivgui()->PostMessage(GetVParent(), msg, GetVPanel());
		}
	}

	return loading;
}

void NewHTML::OnFinishURL(const char *url)
{
	BaseClass::OnFinishURL( url );

	if (GetVParent())
	{
		KeyValues *msg = new KeyValues("FinishedLoading", "url", url);
		ivgui()->PostMessage(GetVParent(), msg, GetVPanel());
	}
}

}; // namespace vgui