
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
	OpenAchievementLocal(OpenAchievement const&) = delete;
	OpenAchievementLocal(OpenAchievementLocal const&) = delete;
	void operator=(OpenAchievement const&) = delete;
	void operator=(OpenAchievementLocal const&) = delete;
private:
	OpenAchievementLocal() {
		this->apiEnabled = false;
	}
	OpenAchievementLocal(bool apiEnabled);
	static OpenAchievementLocal* instance;
	static mutex mutex;
	bool apiEnabled;
};

OpenAchievementLocal* OpenAchievementLocal::instance = nullptr;
mutex OpenAchievementLocal::mutex;

OpenAchievementLocal::OpenAchievementLocal(bool apiEnabled) {
	this->apiEnabled = apiEnabled;
}

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
	if (instance == nullptr) {
		lock_guard<std::mutex> lock(mutex);
		if (instance == nullptr) {
			instance = new OpenAchievementLocal(apiEnabled);
		}
	}
	return instance;
}

OpenAchievement* getAchievementInstance(bool apiEnabled)
{
	return OpenAchievementLocal::getInstance(apiEnabled);
}
