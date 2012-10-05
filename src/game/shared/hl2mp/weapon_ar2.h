//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Projectile shot from the AR2 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef	WEAPONAR2_H
#define	WEAPONAR2_H

#include "basegrenade_shared.h"
#include "weapon_sdkbase.h"

#ifdef CLIENT_DLL
#define CWeaponAR2 C_WeaponAR2
#endif

class CWeaponAR2 : public CWeaponSDKBase
{
public:
	DECLARE_CLASS( CWeaponAR2, CWeaponSDKBase );

	CWeaponAR2();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void	ItemPostFrame( void );
	void	Precache( void );
	
	void	PrimaryAttack( void );
	void	SecondaryAttack( void );
	void	DelayedAttack( void );

	const char *GetTracerType( void ) { return "AR2Tracer"; }

	void	AddViewKick( void );

	int		GetMinBurst( void ) { return 1; }
	int		GetMaxBurst( void ) { return 3; }
	float	GetFireRate( void ) { return 0.15f; }

	bool	CanHolster( void );
	bool	Reload( void );

	Activity	GetPrimaryAttackActivity( void );
	
	void	DoImpactEffect( trace_t &tr, int nDamageType );
	
	virtual bool Deploy( void );


	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone;
		
		cone = VECTOR_CONE_3DEGREES;

		return cone;
	}
	
	const WeaponProficiencyInfo_t *GetProficiencyValues();
	virtual SDKWeaponID GetWeaponID( void ) const		{ return SDK_WEAPON_AR2; }
	void DoMachineGunKick( CBasePlayer *pPlayer, float dampEasy, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime );

private:
	CWeaponAR2( const CWeaponAR2 & );

protected:

	float					m_flDelayedFire;
	bool					m_bShotDelayed;
	int						m_nVentPose;
	
	DECLARE_ACTTABLE();
};


#endif	//WEAPONAR2_H
