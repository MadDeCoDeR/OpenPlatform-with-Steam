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
#include "OpenAchievement.h"
#include "sdk/public/steam/steam_api.h"
#include <mutex>

using namespace std;

class OpenAchievementLocal : public OpenAchievement {
	virtual bool GetAchievement(const char* name, bool* status);
	virtual const char* GetAchievementDevName(unsigned int id);
	virtual bool GetAchievementPercent(const char* name, unsigned int progress, unsigned int max);
	virtual bool UnlockAchievement(const char* name);
	virtual bool LockAchievement(const char* name);
	virtual const char* GetAchievementName(const char* name);
	virtual const char* GetAchievementDescription(const char* name);
	virtual bool GetAchievementHidden(const char* name);
public:
	static OpenAchievement* getInstance(bool apiEnabled);
	OpenAchievementLocal() {
		apiEnabled = false;
	}
private:
	bool apiEnabled;
};

OpenAchievementLocal instance;

bool OpenAchievementLocal::GetAchievement(const char* name, bool* status)
{
	if (apiEnabled)
		return SteamUserStats()->GetAchievement(name, status);
	return false;
}

const char* OpenAchievementLocal::GetAchievementDevName(unsigned int id)
{
	if (apiEnabled)
		return SteamUserStats()->GetAchievementName(id);
	return "";
}

bool OpenAchievementLocal::GetAchievementPercent(const char* name, unsigned int progress, unsigned int max)
{
	if (apiEnabled) {
		if (SteamUserStats()->RequestCurrentStats()) {
			return SteamUserStats()->IndicateAchievementProgress(name, progress, max);
		}
	}
	return false;
}

bool OpenAchievementLocal::UnlockAchievement(const char* name)
{
	if (apiEnabled) {
		if (SteamUserStats()->RequestCurrentStats()) {
			if (SteamUserStats()->SetAchievement(name)) {
				return SteamUserStats()->StoreStats();
			}
		}
	}
	return false;
}

bool OpenAchievementLocal::LockAchievement(const char* name)
{
	if (apiEnabled) {
		if (SteamUserStats()->RequestCurrentStats()) {
			if (SteamUserStats()->ClearAchievement(name)) {
				return SteamUserStats()->StoreStats();
			}
		}
	}
	return false;
}

const char* OpenAchievementLocal::GetAchievementName(const char* name)
{
	if (apiEnabled)
		return SteamUserStats()->GetAchievementDisplayAttribute(name, "name");
	return "";
}

const char* OpenAchievementLocal::GetAchievementDescription(const char* name)
{
	if (apiEnabled)
		return SteamUserStats()->GetAchievementDisplayAttribute(name, "desc");

	return "";
}

bool OpenAchievementLocal::GetAchievementHidden(const char* name)
{
	if (apiEnabled)
		return atoi(SteamUserStats()->GetAchievementDisplayAttribute(name, "hidden"));

	return false;
}

OpenAchievement* OpenAchievementLocal::getInstance(bool apiEnabled)
{
	instance.apiEnabled = apiEnabled;
	return &instance;
}

OpenAchievement* getAchievementInstance(bool apiEnabled)
{
	return OpenAchievementLocal::getInstance(apiEnabled);
}
