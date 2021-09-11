/*
Open Platform

Copyright(C) 2021 George Kalmpokis

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files(the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// OpenPlatfom.cpp : Defines the exported functions for the DLL application.
//

#include "OpenPlatform.h"
#include "sdk/public/steam/steam_api.h"
#include <cstdlib>
#include <iostream>
#include <string>

static bool overlayActive = false;
static unsigned long long LobbyID = 0;

class OPlatformLocal : public OPlatform
{
public:
	virtual bool API_Init();
	virtual bool API_Init(const char* data);
	virtual void API_Shutdown();
	virtual const char* GetPlatformUserName();
	virtual bool GetAchievement(const char* name, bool* status);
	virtual const char* GetAchievementDevName(unsigned int id);
	virtual bool GetAchievementPercent(const char* name, unsigned int progress, unsigned int max);
	virtual bool UnlockAchievement(const char* name);
	virtual bool LockAchievement(const char* name);
	virtual const char* GetAchievementName(const char* name);
	virtual const char* GetAchievementDescription(const char* name);
	virtual bool GetAchievementHidden(const char* name);
	virtual void ShowUser( unsigned int id);
	virtual bool isPlatformOverlayActive();
	virtual void SetNotificationsPosition(unsigned int x, unsigned int y);
	virtual unsigned long long CreateLobby(int type, int maxplayers);
	virtual bool GetCloudStats(unsigned long long* totalBytes, unsigned long long* availableBytes);
	virtual bool SetAdditionalInfo(const char* key, const char* value);
private:
	#ifdef WIN32
	STEAM_CALLBACK(OPlatformLocal, OnGameOverlayActivated, GameOverlayActivated_t);
	#endif
	void OnLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure);
	CCallResult< OPlatformLocal, LobbyCreated_t > m_LobbyCreatedCallResult;
	bool apienabled;
	int32 parseString(const char* string);
};

OPlatformLocal opl;

#if __MWERKS__
#pragma export on
#endif
#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif
extern "C" OPlatform* GetPlatformAPI()
{
#if __MWERKS__
#pragma export off
#endif

	return &opl;
}
#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

OPlatform* op = &opl;

bool OPlatformLocal::API_Init() {
	if (SteamAPI_Init()) {
		apienabled = true;
	}
	else {
		apienabled = false;
	}
	return apienabled;
}

bool OPlatformLocal::API_Init(const char* data)
{
	int32 id = parseString(data);
	if (SteamAPI_RestartAppIfNecessary(id)) {
		apienabled = true;
	}
	else {
		apienabled = false;
	}
	return apienabled;
}

void  OPlatformLocal::API_Shutdown() {
	if (apienabled)
		SteamAPI_Shutdown();
}

const char* OPlatformLocal::GetPlatformUserName() {
	if (apienabled)
		return SteamFriends()->GetPersonaName();

	return "";
}

bool OPlatformLocal::GetAchievement(const char* name, bool* status) {
	if (apienabled)
		return SteamUserStats()->GetAchievement(name, status);

	return false;
}

const char* OPlatformLocal::GetAchievementDevName(unsigned int id) {
	if (apienabled)
		return SteamUserStats()->GetAchievementName(id);

	return "";
}

bool OPlatformLocal::GetAchievementPercent(const char* name, unsigned int progress, unsigned int max) {
	if (apienabled) {
		if (SteamUserStats()->RequestCurrentStats()) {
			return SteamUserStats()->IndicateAchievementProgress(name, progress, max);
		}
	}
	return false;
}

bool OPlatformLocal::UnlockAchievement(const char* name) {
	if (apienabled) {
		if (SteamUserStats()->RequestCurrentStats()) {
			if (SteamUserStats()->SetAchievement(name)) {
				return SteamUserStats()->StoreStats();
			}
		}
	}
	return false;
}

bool OPlatformLocal::LockAchievement(const char* name) {
	if (apienabled) {
		if (SteamUserStats()->RequestCurrentStats()) {
			if (SteamUserStats()->ClearAchievement(name)) {
				return SteamUserStats()->StoreStats();
			}
		}
	}
	return false;
}

const char* OPlatformLocal::GetAchievementName(const char* name) {
	if (apienabled)
		return SteamUserStats()->GetAchievementDisplayAttribute(name, "name");

	return "";
}

const char* OPlatformLocal::GetAchievementDescription(const char* name) {
	if (apienabled)
		return SteamUserStats()->GetAchievementDisplayAttribute(name, "desc");

	return "";
}

bool OPlatformLocal::GetAchievementHidden(const char* name) {
	if (apienabled)
		return atoi(SteamUserStats()->GetAchievementDisplayAttribute(name, "hidden"));

	return false;
}

 void OPlatformLocal::ShowUser( unsigned int id) {
	 if (apienabled)
		 SteamFriends()->ActivateGameOverlayToUser("steamid", CSteamID(id,k_EUniversePublic,k_EAccountTypeIndividual));
}

 bool OPlatformLocal::isPlatformOverlayActive() {
	 if (apienabled)
		SteamAPI_RunCallbacks();

	 return ::overlayActive;
}
#ifdef WIN32
 void OPlatformLocal::OnGameOverlayActivated(GameOverlayActivated_t* pCallback) {
	 ::overlayActive = pCallback->m_bActive;
}
#endif

 void OPlatformLocal::SetNotificationsPosition(unsigned int x, unsigned int y) 
 {
	 ENotificationPosition pos = k_EPositionBottomRight;
	 switch (x){
	 case 0:
		 if (!y) {
			 pos = k_EPositionTopLeft;
		 }
		 else {
			 pos = k_EPositionBottomLeft;
		 }
		 break;
	 default:
		 if (!y) {
			 pos = k_EPositionTopRight;
		 }
		 else {
			 pos = k_EPositionBottomRight;
		 }
		 break;
	 }
	 if (apienabled)
		 SteamUtils()->SetOverlayNotificationPosition(pos);
 }

 unsigned long long OPlatformLocal::CreateLobby(int type, int maxplayers) {
	 if (apienabled) {
		 SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby((ELobbyType)type, maxplayers);
		 m_LobbyCreatedCallResult.Set(hSteamAPICall, this, &OPlatformLocal::OnLobbyCreated);
		 bool fail = false;
		 SteamAPI_RunCallbacks();
		 LobbyCreated_t pCallback;
		 while (!SteamUtils()->GetAPICallResult(hSteamAPICall,&pCallback,sizeof(pCallback),pCallback.k_iCallback, &fail)) {
		 }
		 ::LobbyID = pCallback.m_ulSteamIDLobby;
		 return ::LobbyID;
	 }
	 return 0;
 }

 bool OPlatformLocal::GetCloudStats(unsigned long long* totalBytes, unsigned long long* availableBytes)
 {
	 return SteamRemoteStorage()->GetQuota(totalBytes, availableBytes);
 }

 bool OPlatformLocal::SetAdditionalInfo(const char* key, const char* value)
 {
	 return SteamFriends()->SetRichPresence(key, value);
 }

 void OPlatformLocal::OnLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure) {
	 if (bIOFailure || pCallback->m_eResult != k_EResultOK) {
		 return;
	 }
 }

 
int32 OPlatformLocal::parseString(const char* string)
 {
	int32 result = 0;
	int size = strlen(string);
	int allDigit = size;
	for (int i = 0; i < size; i++) {
		if (std::isdigit(string[i])) {
			allDigit = allDigit - 1;
		}
	}
	if (allDigit == 0) {
		result = std::stoi(string);
	}
	return result;
 }
