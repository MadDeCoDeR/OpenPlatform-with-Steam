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

class OPlatformLocal : public OPlatform
{
public:
	virtual const char* API_Name();
	virtual bool API_Init();
	virtual bool API_Init(const char* data);
	virtual void API_Shutdown();
	virtual OpenAchievement* openAchievement();
	virtual OpenDLC* openDLC();
	virtual OpenApp* openApp();
	virtual OpenInput* openInput();
	virtual OpenMultiplayer* openMultiplayer();
	virtual const char* GetPlatformUserName();
	virtual void ShowUser( unsigned int id);
	virtual bool API_pump();
	virtual void SetNotificationsPosition(unsigned int x, unsigned int y);
	virtual bool SetAdditionalInfo(const char* key, const char* value);
	virtual bool IsPortable();
	virtual unsigned char GetBatteryLevel();
	virtual bool ShowFloatingTextBox(int type, int xpos, int ypos, int width, int height);
public:
	OPlatformLocal() {
		apiEnabled = false;
	}
private:
	STEAM_CALLBACK_MANUAL(OPlatformLocal, OnGameOverlayActivated, GameOverlayActivated_t, m_OverlayActivatedCallBack);
	bool apiEnabled;
	
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

const char* OPlatformLocal::API_Name()
{
	return "Steam";
}

bool OPlatformLocal::API_Init() { 
	apiEnabled = SteamAPI_Init();
	return apiEnabled;
}
bool OPlatformLocal::API_Init(const char* data)
{
	int32 id = parseString(data);
	apiEnabled = SteamAPI_RestartAppIfNecessary(id);
	return apiEnabled;
}
void  OPlatformLocal::API_Shutdown() {
	if (apiEnabled) {
		SteamAPI_Shutdown();
		
	}
}
OpenAchievement* OPlatformLocal::openAchievement()
{
	return getAchievementInstance(apiEnabled);
}
OpenDLC* OPlatformLocal::openDLC()
{
	return getDLCInstance(apiEnabled);
}
OpenApp* OPlatformLocal::openApp()
{
	return getAppInstance(apiEnabled);
}
OpenInput* OPlatformLocal::openInput()
{
	return getInputInstance(apiEnabled);
}
OpenMultiplayer* OPlatformLocal::openMultiplayer()
{
	return getMultiplayerInstance(apiEnabled);
}
const char* OPlatformLocal::GetPlatformUserName() {
	if (apiEnabled)
		return SteamFriends()->GetPersonaName();

	return "";
}
void OPlatformLocal::ShowUser( unsigned int id) {
	if (apiEnabled)
		SteamFriends()->ActivateGameOverlayToUser("steamid", CSteamID(id, k_EUniversePublic, k_EAccountTypeIndividual));
}
bool OPlatformLocal::API_pump() {
	if (apiEnabled)
		SteamAPI_RunCallbacks();

	return ::overlayActive;
}
void OPlatformLocal::OnGameOverlayActivated(GameOverlayActivated_t* pCallback) {
	::overlayActive = pCallback->m_bActive;
}

void OPlatformLocal::SetNotificationsPosition(unsigned int x, unsigned int y) {
	ENotificationPosition pos = k_EPositionBottomRight;
	switch (x) {
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
	if (apiEnabled)
		SteamUtils()->SetOverlayNotificationPosition(pos);
}

bool OPlatformLocal::SetAdditionalInfo(const char* key, const char* value)
{
	if (apiEnabled) {
		return SteamFriends()->SetRichPresence(key, value);
	}
	return false;
}

bool OPlatformLocal::IsPortable()
{
	if (apiEnabled) {
		return SteamUtils()->IsSteamRunningOnSteamDeck();
	}
	return false;
}

unsigned char OPlatformLocal::GetBatteryLevel()
{
	if (apiEnabled)
		return SteamUtils()->GetCurrentBatteryPower();

	return 0;
}

bool OPlatformLocal::ShowFloatingTextBox(int type, int xpos, int ypos, int width, int height)
{
	if (apiEnabled) {
		if (type < EFloatingGamepadTextInputMode::k_EFloatingGamepadTextInputModeModeNumeric + 1) {
			return SteamUtils()->ShowFloatingGamepadTextInput(static_cast<EFloatingGamepadTextInputMode>(type), xpos, ypos, width, height);
		}
	}
	return false;
}


int32 OPlatformLocal::parseString(const char* string)
{
	int32 result = 0;
	size_t size = strlen(string);
	size_t allDigit = size;
	for (unsigned int i = 0; i < size; i++) {
		if (std::isdigit(string[i])) {
			allDigit = allDigit - 1;
		}
	}
	if (allDigit == 0) {
		result = std::stoi(string);
	}
	return result;
}