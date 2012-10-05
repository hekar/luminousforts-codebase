//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef SDK_PLAYERANIMSTATE_H
#define SDK_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif


#include "convar.h"
#include "multiplayer_animstate.h"

#if defined( CLIENT_DLL )
class C_SDKPlayer;
#define CSDKPlayer C_SDKPlayer
#else
class CSDKPlayer;
#endif

#define HL2MP_PLAYERANIMATIONS
//#define SDK_PLAYERANIMATIONS

// ------------------------------------------------------------------------------------------------ //
// CPlayerAnimState declaration.
// ------------------------------------------------------------------------------------------------ //
class CSDKPlayerAnimState : public CMultiPlayerAnimState
{
public:
	
	DECLARE_CLASS( CSDKPlayerAnimState, CMultiPlayerAnimState );

	CSDKPlayerAnimState();
	CSDKPlayerAnimState( CBasePlayer *pPlayer, MultiPlayerMovementData_t &movementData );
	~CSDKPlayerAnimState();

	void InitSDKAnimState( CSDKPlayer *pPlayer );
	CSDKPlayer *GetSDKPlayer( void )							{ return m_pSDKPlayer; }

	virtual void ClearAnimationState();
	virtual Activity TranslateActivity( Activity actDesired );
	virtual void Update( float eyeYaw, float eyePitch );

	void	DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );

	bool	HandleMoving( Activity &idealActivity );
	bool	HandleJumping( Activity &idealActivity );
	bool	HandleDucking( Activity &idealActivity );
	bool	HandleSwimming( Activity &idealActivity );

#if defined ( SDK_USE_SPRINTING )
	bool	HandleSprinting( Activity &idealActivity );
#endif

#ifdef SDK_PLAYERANIMATIONS
	//Tony; overriding because the SDK Player models pose parameter is flipped the opposite direction
	virtual void		ComputePoseParam_MoveYaw( CStudioHdr *pStudioHdr );
#endif

#ifdef HL2MP_PLAYERANIMATIONS
	virtual float GetCurrentMaxGroundSpeed();
private:
	//Tony; temp till 9way!
	bool						SetupPoseParameters( CStudioHdr *pStudioHdr );
	virtual void				EstimateYaw( void );
	virtual void				ComputePoseParam_MoveYaw( CStudioHdr *pStudioHdr );
	virtual void				ComputePoseParam_AimPitch( CStudioHdr *pStudioHdr );
	virtual void				ComputePoseParam_AimYaw( CStudioHdr *pStudioHdr );
#endif

	virtual Activity CalcMainActivity();	

private:
	
	CSDKPlayer   *m_pSDKPlayer;
	bool		m_bInAirWalk;

	float		m_flHoldDeployedPoseUntilTime;
};

CSDKPlayerAnimState *CreateSDKPlayerAnimState( CSDKPlayer *pPlayer );



#endif // SDK_PLAYERANIMSTATE_H
