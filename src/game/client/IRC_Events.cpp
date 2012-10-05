#include "cbase.h"

#ifdef MOD_IRC

#include "IRC_Events.h"

void event_join (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{

}

void event_connect (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{

}

void event_privmsg (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
}

void event_channel (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{

}

// huh?
void event_numeric (irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count)
{
}

// huh?
void irc_event_dcc_chat (irc_session_t * session, const char * nick, const char * addr, irc_dcc_t dccid)
{
}

// huh?
void irc_event_dcc_send (irc_session_t * session, const char * nick, const char * addr, const char * filename, unsigned long size, irc_dcc_t dccid)
{
}

// Event that does nothing but dump parameters and information
void dummy_event( irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count )
{
	// Do nothing...
	Msg( "IRC Event: %s\n", event );
	Msg( "IRC Origin: %s\n", origin );
	for ( unsigned int i = 0; i < count; i++ )
	{
		Msg( "\tParam %u: %s\n", i + 1, params[ i ] );
	}
}

#endif // MOD_IRC