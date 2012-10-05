#ifndef WP_INCLUDES
	THROWERROR ("WP_PROTOTYPES IMPROPERLY ADDED")
#endif

//-----------------------------------------------------------------------------
// WARNING:
// ONLY ADD ONCE IN WEAPON_PHYSCANNON.CPP
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------------------------------
//  CWeaponPhysCannon class
//----------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef CLIENT_DLL
#define CWeaponPhysCannon C_WeaponPhysCannon
#endif

class CWeaponPhysCannon : public CWeaponSDKBase
{
public:
	DECLARE_CLASS( CWeaponPhysCannon, CWeaponSDKBase );

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CWeaponPhysCannon( void );
	~CWeaponPhysCannon ();

	void	Drop( const Vector &vecVelocity );
	void	Precache();
	virtual void	OnRestore();
	virtual void	StopLoopingSounds();
	virtual void	UpdateOnRemove(void);

	virtual SDKWeaponID GetWeaponID( void ) const { return SDK_WEAPON_PHYSCANNON; }

	void	PrimaryAttack();
	void	SecondaryAttack();
	void	WeaponIdle();
	void	ItemPreFrame();
	void	ItemPostFrame();

	void	ForceDrop( void );
	bool	DropIfEntityHeld( CBaseEntity *pTarget );	// Drops its held entity if it matches the entity passed in
	CGrabController &GetGrabController() { return m_grabController; }

	bool	CanHolster( void );
	bool	Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	bool	Deploy( void );

	bool	HasAnyAmmo( void ) { return true; }

	virtual void SetViewModel( void );
	virtual const char *GetShootSound( int iIndex ) const;
	
#ifndef CLIENT_DLL
	CNetworkQAngle	( m_attachedAnglesPlayerSpace );
#else
	QAngle m_attachedAnglesPlayerSpace;
#endif

	CNetworkVector	( m_attachedPositionObjectSpace );

	CNetworkHandle( CBaseEntity, m_hAttachedObject );

	EHANDLE m_hOldAttachedObject;

protected:
	enum FindObjectResult_t
	{
		OBJECT_FOUND = 0,
		OBJECT_NOT_FOUND,
		OBJECT_BEING_DETACHED,
	};

	void	DoEffect( int effectType, Vector *pos = NULL );

	void	OpenElements( void );
	void	CloseElements( void );

	// Pickup and throw objects.
	bool	CanPickupObject( CBaseEntity *pTarget );
	void	CheckForTarget( void );

	float m_NextFreeze;
	
#ifndef CLIENT_DLL
	bool	AttachObject( CBaseEntity *pObject, const Vector &vPosition );
	FindObjectResult_t		FindObject( void );
	CBaseEntity *FindObjectInCone( const Vector &vecOrigin, const Vector &vecDir, float flCone );
#endif	// !CLIENT_DLL

	void	UpdateObject( void );
	void	DetachObject( bool playSound = true, bool wasLaunched = false );
	void	LaunchObject( const Vector &vecDir, float flForce );
	void	StartEffects( void );	// Initialize all sprites and beams
	void	StopEffects( bool stopSound = true );	// Hide all effects temporarily
	void	DestroyEffects( void );	// Destroy all sprites and beams

	// Punt objects - this is pointing at an object in the world and applying a force to it.
	void	PuntNonVPhysics( CBaseEntity *pEntity, const Vector &forward, trace_t &tr );
	void	PuntVPhysics( CBaseEntity *pEntity, const Vector &forward, trace_t &tr );

	// Velocity-based throw common to punt and launch code.
	void	ApplyVelocityBasedForce( CBaseEntity *pEntity, const Vector &forward );

	// Physgun effects
	void	DoEffectClosed( void );
	void	DoEffectReady( void );
	void	DoEffectHolding( void );
	void	DoEffectLaunch( Vector *pos );
	void	DoEffectNone( void );
	void	DoEffectIdle( void );

	// Trace length
	float	TraceLength();

	// Sprite scale factor 
	float	SpriteScaleFactor();

	float			GetLoadPercentage();
	CSoundPatch		*GetMotorSound( void );

	void	DryFire( void );
	void	PrimaryFireEffect( void );

#ifndef CLIENT_DLL
	// What happens when the physgun picks up something 
	void	Physgun_OnPhysGunPickup( CBaseEntity *pEntity, CBasePlayer *pOwner, PhysGunPickup_t reason );
#endif	// !CLIENT_DLL

#ifdef CLIENT_DLL

	enum EffectType_t
	{
		PHYSCANNON_CORE = 0,
		
		PHYSCANNON_BLAST,

		PHYSCANNON_GLOW1,	// Must be in order!
		PHYSCANNON_GLOW2,
		PHYSCANNON_GLOW3,
		PHYSCANNON_GLOW4,
		PHYSCANNON_GLOW5,
		PHYSCANNON_GLOW6,

		PHYSCANNON_ENDCAP1,	// Must be in order!
		PHYSCANNON_ENDCAP2,
		PHYSCANNON_ENDCAP3,	// Only used in third-person!

		NUM_PHYSCANNON_PARAMETERS	// Must be last!
	};

#define	NUM_GLOW_SPRITES ((CWeaponPhysCannon::PHYSCANNON_GLOW6-CWeaponPhysCannon::PHYSCANNON_GLOW1)+1)
#define NUM_ENDCAP_SPRITES ((CWeaponPhysCannon::PHYSCANNON_ENDCAP3-CWeaponPhysCannon::PHYSCANNON_ENDCAP1)+1)

#define	NUM_PHYSCANNON_BEAMS	3

	virtual int		DrawModel( int flags );
	virtual void	ViewModelDrawn( C_BaseViewModel *pBaseViewModel );
	virtual bool	IsTransparent( void );
	virtual void	OnDataChanged( DataUpdateType_t type );
	virtual void	ClientThink( void );
	
	virtual bool	IsPredicted() const { return true; }
	void			DrawEffects( void );
	void			GetEffectParameters( EffectType_t effectID, color32 &color, float &scale, IMaterial **pMaterial, Vector &vecAttachment );
	void			DrawEffectSprite( EffectType_t effectID );
	inline bool		IsEffectVisible( EffectType_t effectID );
	void			UpdateElementPosition( void );

	// We need to render opaque and translucent pieces
	RenderGroup_t	GetRenderGroup( void ) {	return RENDER_GROUP_TWOPASS;	}

	CInterpolatedValue		m_ElementParameter;							// Used to interpolate the position of the articulated elements
	CPhysCannonEffect		m_Parameters[NUM_PHYSCANNON_PARAMETERS];	// Interpolated parameters for the effects
	CPhysCannonEffectBeam	m_Beams[NUM_PHYSCANNON_BEAMS];				// Beams

	int				m_nOldEffectState;	// Used for parity checks
	bool			m_bOldOpen;			// Used for parity checks

	void			ManagePredictedObject( void );
	void			NotifyShouldTransmit( ShouldTransmitState_t state );

#endif	// CLIENT_DLL


	int		m_nChangeState;				// For delayed state change of elements
	float	m_flCheckSuppressTime;		// Amount of time to suppress the checking for targets
	bool	m_flLastDenySoundPlayed;	// Debounce for deny sound
	int		m_nAttack2Debounce;

	CNetworkVar( bool,	m_bActive );
	CNetworkVar( int,	m_EffectState );		// Current state of the effects on the gun
	CNetworkVar( bool,	m_bOpen );

	bool	m_bResetOwnerEntity;
	
	float	m_flElementDebounce;

	CSoundPatch			*m_sndMotor;		// Whirring sound for the gun
	
	CGrabController		m_grabController;

	float	m_flRepuntObjectTime;
	EHANDLE m_hLastPuntedObject;

private:
	CWeaponPhysCannon( const CWeaponPhysCannon & );

	DECLARE_ACTTABLE();
};


IMPLEMENT_NETWORKCLASS_ALIASED( WeaponPhysCannon, DT_WeaponPhysCannon )

BEGIN_NETWORK_TABLE( CWeaponPhysCannon, DT_WeaponPhysCannon )
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bActive ) ),
	RecvPropEHandle( RECVINFO( m_hAttachedObject ) ),
	RecvPropVector( RECVINFO( m_attachedPositionObjectSpace ) ),
	RecvPropFloat( RECVINFO( m_attachedAnglesPlayerSpace[0] ) ),
	RecvPropFloat( RECVINFO( m_attachedAnglesPlayerSpace[1] ) ),
	RecvPropFloat( RECVINFO( m_attachedAnglesPlayerSpace[2] ) ),
	RecvPropInt( RECVINFO( m_EffectState ) ),
	RecvPropBool( RECVINFO( m_bOpen ) ),
#else
	SendPropBool( SENDINFO( m_bActive ) ),
	SendPropEHandle( SENDINFO( m_hAttachedObject ) ),
	SendPropVector(SENDINFO( m_attachedPositionObjectSpace ), -1, SPROP_COORD),
	SendPropAngle( SENDINFO_VECTORELEM(m_attachedAnglesPlayerSpace, 0 ), 11 ),
	SendPropAngle( SENDINFO_VECTORELEM(m_attachedAnglesPlayerSpace, 1 ), 11 ),
	SendPropAngle( SENDINFO_VECTORELEM(m_attachedAnglesPlayerSpace, 2 ), 11 ),
	SendPropInt( SENDINFO( m_EffectState ) ),
	SendPropBool( SENDINFO( m_bOpen ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponPhysCannon )
	DEFINE_PRED_FIELD( m_EffectState,	FIELD_INTEGER,	FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bOpen,			FIELD_BOOLEAN,	FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_physcannon, CWeaponPhysCannon );
PRECACHE_WEAPON_REGISTER( weapon_physcannon );

acttable_t	CWeaponPhysCannon::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_PHYSGUN,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_PHYSGUN,			false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_PHYSGUN,					false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_PHYSGUN,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PHYSGUN,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PHYSGUN,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_PHYSGUN,		false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_PHYSGUN,		false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_PHYSGUN,					false },
};

IMPLEMENT_ACTTABLE(CWeaponPhysCannon);

enum
{
	ELEMENT_STATE_NONE = -1,
	ELEMENT_STATE_OPEN,
	ELEMENT_STATE_CLOSED,
};

enum
{
	EFFECT_NONE,
	EFFECT_CLOSED,
	EFFECT_READY,
	EFFECT_HOLDING,
	EFFECT_LAUNCH,
};