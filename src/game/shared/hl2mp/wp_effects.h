#ifndef WP_INCLUDES
	THROWERROR ("WP_EFFECTS IMPROPERLY ADDED")
#endif

//-----------------------------------------------------------------------------
// WARNING:
// ONLY ADD ONCE IN WEAPON_PHYSCANNON.CPP
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------------------------------
//  CInterpolatedValue class
//----------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef CLIENT_DLL

//----------------------------------------------------------------------------------------------------------------------------------------------------------
//  CPhysCannonEffect class
//----------------------------------------------------------------------------------------------------------------------------------------------------------

class CPhysCannonEffect
{
public:
	CPhysCannonEffect( void ) : m_vecColor( 255, 255, 255 ), m_bVisible( true ), m_nAttachment( -1 ) {};

	void SetAttachment( int attachment ) { m_nAttachment = attachment; }
	int	GetAttachment( void ) const { return m_nAttachment; }

	void SetVisible( bool visible = true ) { m_bVisible = visible; }
	int IsVisible( void ) const { return m_bVisible; }

	void SetColor( const Vector &color ) { m_vecColor = color; }
	const Vector &GetColor( void ) const { return m_vecColor; }

	bool SetMaterial(  const char *materialName )
	{
		m_hMaterial.Init( materialName, TEXTURE_GROUP_CLIENT_EFFECTS );
		return ( m_hMaterial != NULL );
	}

	CMaterialReference &GetMaterial( void ) { return m_hMaterial; }

	CInterpolatedValue &GetAlpha( void ) { return m_Alpha; }
	CInterpolatedValue &GetScale( void ) { return m_Scale; }

private:
	CInterpolatedValue	m_Alpha;
	CInterpolatedValue	m_Scale;

	Vector				m_vecColor;
	bool				m_bVisible;
	int					m_nAttachment;
	CMaterialReference	m_hMaterial;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------
//  CPhysCannonEffectBeam class
//----------------------------------------------------------------------------------------------------------------------------------------------------------

class CPhysCannonEffectBeam
{
public:
	CPhysCannonEffectBeam( void ) : m_pBeam( NULL ) {};

	~CPhysCannonEffectBeam( void )
	{
		Release();
	}

	void Release( void )
	{
		if ( m_pBeam != NULL )
		{
			m_pBeam->flags = 0;
			m_pBeam->die = gpGlobals->curtime - 1;
			
			m_pBeam = NULL;
		}
	}

	void Init( int startAttachment, int endAttachment, CBaseEntity *pEntity, bool firstPerson )
	{
		if ( m_pBeam != NULL )
			return;

		BeamInfo_t beamInfo;

		beamInfo.m_pStartEnt = pEntity;
		beamInfo.m_nStartAttachment = startAttachment;
		beamInfo.m_pEndEnt = pEntity;
		beamInfo.m_nEndAttachment = endAttachment;
		beamInfo.m_nType = TE_BEAMPOINTS;
		beamInfo.m_vecStart = vec3_origin;
		beamInfo.m_vecEnd = vec3_origin;
		
		beamInfo.m_pszModelName = ( firstPerson ) ? PHYSCANNON_BEAM_SPRITE_NOZ : PHYSCANNON_BEAM_SPRITE;
		
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = 0.0f;
		
		if ( firstPerson )
		{
			beamInfo.m_flWidth = 0.0f;
			beamInfo.m_flEndWidth = 4.0f;
		}
		else
		{
			beamInfo.m_flWidth = 0.5f;
			beamInfo.m_flEndWidth = 2.0f;
		}

		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 16;
		beamInfo.m_flBrightness = 255.0;
		beamInfo.m_flSpeed = 150.0f;
		beamInfo.m_nStartFrame = 0.0;
		beamInfo.m_flFrameRate = 30.0;
		beamInfo.m_flRed = 255.0;
		beamInfo.m_flGreen = 255.0;
		beamInfo.m_flBlue = 255.0;
		beamInfo.m_nSegments = 8;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = FBEAM_FOREVER;
	
		m_pBeam = beams->CreateBeamEntPoint( beamInfo );
	}

	void SetVisible( bool state = true )
	{
		if ( m_pBeam == NULL )
			return;

		m_pBeam->brightness = ( state ) ? 255.0f : 0.0f;
	}

private:
	Beam_t	*m_pBeam;
};

#endif
