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


#ifndef CBASEGAMERULE_H
#define CBASEGAMERULE_H

#ifdef _WIN32
#pragma once
#endif

/*

class CBasePlayerClass;
class CMultiPlayRules;
class CSDKPlayer;

class IGameRuleInterface
{
public:

	virtual void InitializeRules () = 0;
	virtual void ScanEntity (CBaseEntity *pEntity) = 0;
	virtual void CreatePlayerClassTables () = 0;
	virtual bool AllowPlayerClass (CBasePlayerClass *pClass) = 0;
	virtual bool AllowPlayerClass (PlayerClassSlots_t Slot) = 0;

	virtual void PlayerSpawn (CBasePlayer *pPlayer) = 0;
	virtual bool IsAllowedToSpawn (CBaseEntity *pEntity) = 0;
	virtual bool ItemShouldRespawn( CItem *pItem ) = 0;
	virtual bool CheckWinningConditions () = 0;
	virtual bool GotoIntermission () = 0;
	virtual bool IsIntermission () = 0;
	virtual bool IsGameOver () = 0;

};

class CBaseGameRule : public IGameRuleInterface
{
public:
	CBaseGameRule (CMultiplayRules *pMPRules) { m_bIntermission = false; m_bGameOver = false;};
	virtual ~CBaseGameRule () {};

	virtual void InitializeRules () {};
	virtual void ScanEntity (CBaseEntity *pEntity) {};
	virtual void CreatePlayerClassTables () {};
	virtual bool AllowPlayerClass (CBasePlayerClass *pClass) {return false;};
	virtual bool AllowPlayerClass (PlayerClassSlots_t Slot) {return false;};
	virtual CBasePlayerClass *CreatePlayerClass (CSDKPlayer *pPlayer, PlayerClassSlots_t Slot) {return NULL;};
	virtual void SetPlayerClass (CSDKPlayer *pPlayer, PlayerClassSlots_t Slot) {};

	virtual void PlayerThink (CBasePlayer *pPlayer) {};
	virtual void PlayerSpawn (CBasePlayer *pPlayer) {};
	virtual bool IsAllowedToSpawn (CBaseEntity *pEntity) {return false;};
	virtual bool ItemShouldRespawn(CItem *pItem) {return true;};
	virtual bool CheckWinningConditions () {return false;};
	virtual bool GotoIntermission () {return false;};
	virtual bool IsIntermission () {return m_bIntermission;};
	virtual bool IsGameOver () {return *m_bGameOver;};
	
	virtual int GetGamePhase () {return *m_pGamePhase;};
	virtual void SetGamePhase (int *pGamePhase) {m_pGamePhase = pGamePhase;};

	virtual void SetGameOver (bool *pGameOver) {m_bGameOver = pGameOver;};

protected:
	CMultiPlayRules *m_pMPRules;

	bool m_bIntermission;
	bool *m_bGameOver;
	int *m_pGamePhase;
};

class CCTFGameRule : public CBaseGameRule
{
public:
	CCTFGameRule (CMultiplayRules *pMPRules);
	virtual ~CCTFGameRule ();

	virtual void InitializeRules ();
	virtual void ScanEntity (CBaseEntity *pEntity);
	virtual void CreatePlayerClassTables ();
	virtual bool AllowPlayerClass (CBasePlayerClass *pClass);
	virtual bool AllowPlayerClass (PlayerClassSlots_t Slot);
	virtual CBasePlayerClass *CreatePlayerClass (CSDKPlayer *pPlayer, int Slot);
	virtual void SetPlayerClass (CSDKPlayer *pPlayer, int Slot);

	virtual void PlayerThink (CBasePlayer *pPlayer);
	virtual void PlayerSpawn (CBasePlayer *pPlayer);
	virtual bool IsAllowedToSpawn (CBaseEntity *pEntity);
	virtual bool CheckWinningConditions ();
	virtual bool GotoIntermission ();

private:

};

*/
#endif