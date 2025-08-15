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
#include"OpenDLC.h"
#include "sdk/public/steam/steam_api.h"
#include <mutex>
#include <cstdlib>

class OpenDLCLocal : public OpenDLC {
	virtual int CountDLCs();
	virtual OpenContent GetDLCByIndex(int index);
	virtual bool HasDLC(const char* id);
	virtual void installDLC(const char* id);
public:
	static OpenDLC* getInstance(bool apiEnabled);
	OpenDLCLocal() {
		this->apiEnabled = false;
	}
private:
	
	bool apiEnabled;
};

OpenDLCLocal dlcinstance;

int OpenDLCLocal::CountDLCs()
{
	if (apiEnabled)
		return SteamApps()->GetDLCCount();
	return 0;
}

OpenContent OpenDLCLocal::GetDLCByIndex(int index)
{
	AppId_t id;
	bool available;
	char name[256];
	OpenContent* content = new OpenContent();
	if (apiEnabled && SteamApps()->BGetDLCDataByIndex(index, &id, &available, name, 256)) {
		content->id = new char[256];
		sprintf(content->id, "%u\0", id);
		content->name = new char[256];
		strcpy(content->name, name);
	}
	return *content;
}

bool OpenDLCLocal::HasDLC(const char* id)
{
	unsigned int intId = atoi(id);
	if (apiEnabled)
		return SteamApps()->BIsDlcInstalled(intId);

	return false;
}

void OpenDLCLocal::installDLC(const char* id) {
	unsigned int intId = atoi(id);
	if (apiEnabled)
		SteamApps()->InstallDLC(intId);

}

OpenDLC* OpenDLCLocal::getInstance(bool apiEnabled)
{
	dlcinstance.apiEnabled = apiEnabled;
	return &dlcinstance;
}

OpenDLC* getDLCInstance(bool apiEnabled)
{
	return OpenDLCLocal::getInstance(apiEnabled);
}
