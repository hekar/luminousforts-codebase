/* ***** BEGIN LICENSE BLOCK *****
Version: MPL 1.1/LGPL 2.1/GPL 2.0

The contents of this file are subject to the Mozilla Public License Version 
1.1 (the "License"); you may not use this file except in compliance with
...
for the specific language governing rights and limitations under the
License.

The Original Code is for Luminous Forts.

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


#include "cbase.h"
#include "player.h"
#include "sdk_player.h"
#include "sdk_player_shared.h"
#include "CBasePlayerClass.h"

WeaponList_t g_WeaponList = 
{
	{"weapon_crowbar", NULL, NULL},
	{"weapon_stunstick", NULL, NULL},
	{"weapon_pistol", "Pistol", NULL},
	{"weapon_357", "357", NULL },
	{"weapon_smg1", "SMG1", "SMG1_Grenade"},
	{"weapon_ar2", "AR2", "AR2AltFire"},
	{"weapon_shotgun", "BuckShot", NULL},
	{"weapon_frag", "Grenade", NULL},
	{"weapon_crossbow", "XBowBolt", NULL},
	{"weapon_rpg", "RPG_Round", NULL},
	{"weapon_slam", "slam", NULL},
	{"weapon_physcannon", NULL, NULL}	
};

CBasePlayerClass::CBasePlayerClass (CBasePlayer *pPlayer, char *Name)
{
	m_pPlayer = pPlayer;
	SetName (Name);
}

CBasePlayerClass::~CBasePlayerClass ()
{
	delete m_czName;
}

void CBasePlayerClass::AddWeapon (WeaponDefines_t Weapon)
{
	m_pPlayer->GiveNamedItem (g_WeaponList[Weapon].WeaponName);
}

void CBasePlayerClass::AddAmmo (char *AmmoName, int iCount)
{
	m_pPlayer->GiveAmmo (iCount, AmmoName);
}

void CBasePlayerClass::AddWeapon (WeaponDefines_t Weapon, int iAmmoCount, int iSecAmmoCount = 0)
{
	m_pPlayer->GiveAmmo (iAmmoCount, g_WeaponList[Weapon].PriAmmoName);
	if (iSecAmmoCount > 0)
	{
		m_pPlayer->GiveAmmo (iAmmoCount, g_WeaponList[Weapon].SecAmmoName);
	}

	m_pPlayer->GiveNamedItem (g_WeaponList[Weapon].WeaponName);
}

void CBasePlayerClass::ChangeSpeed (float Speed)
{
	m_pPlayer->SetMaxSpeed (Speed);
}

void CBasePlayerClass::ChangeSprint (float Sprint)
{
	//m_pPlayer->m_HL2Local.m_flSuitPower = 
}

void CBasePlayerClass::ChangeHealth (int Health, int Armor)
{
	m_pPlayer->SetHealth (Health);
	m_pPlayer->IncrementArmorValue( Armor, 150 );
}

void CBasePlayerClass::SetName (char *Name)
{
	m_czName = new char [Q_strlen (Name)];
	Q_strcpy (m_czName, Name);
}

char *CBasePlayerClass::GetName ()
{
	return m_czName;
}

CClassicCTFPlayerClass::CClassicCTFPlayerClass (CBasePlayer *pPlayer, char *Name) : CBasePlayerClass (pPlayer, Name)
{
	AddWeapon (WPN_CROWBAR);
	AddWeapon (WPN_PISTOL, 75, 0);
	AddWeapon (WPN_GRENADE, 5, 0);

	//ChangeSprint ();
	//ChangeSpeed ();
	//ChangeHealth (100, 0);
}

CScoutPlayerClass::CScoutPlayerClass (CBasePlayer *pPlayer) : CClassicCTFPlayerClass (pPlayer, "Scout")
{
	AddWeapon (WPN_SMG, 90, 0);
}

CEngineerPlayerClass::CEngineerPlayerClass (CBasePlayer *pPlayer) : CClassicCTFPlayerClass (pPlayer, "Engineer")
{
	AddWeapon (WPN_SHOTGUN, 30, 0);
	AddWeapon (WPN_PHYSCANNON);
}

CSoldierPlayerClass::CSoldierPlayerClass (CBasePlayer *pPlayer) : CClassicCTFPlayerClass (pPlayer, "Soldier")
{
	AddWeapon (WPN_AR2, 270, 0);
}

CRocketeerPlayerClass::CRocketeerPlayerClass (CBasePlayer *pPlayer) : CClassicCTFPlayerClass (pPlayer, "Rocketeer")
{
	AddWeapon (WPN_RPG, 5, 0);
	AddWeapon (WPN_SLAM, 3, 0);
}

CSniperPlayerClass::CSniperPlayerClass (CBasePlayer *pPlayer) : CClassicCTFPlayerClass (pPlayer, "Sniper")
{
	AddWeapon (WPN_CROSSBOW, 10, 0);
}

