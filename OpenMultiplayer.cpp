/*
Open Platform

Copyright(C) 2023 George Kalampokis

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
#include "OpenMultiplayer.h"
#include "sdk/public/steam/steam_api.h"



class OpenMultiplayerLocal : public OpenMultiplayer {
	virtual void CreateLobby(int type, int maxplayers);
	virtual int CountLobbyUsers();
	virtual unsigned long long GetLobbyUserId(int index);
	virtual void JoinLobby(unsigned long long id);
	virtual void LeaveLobby();
	virtual bool SendPacket(unsigned long long userId, const void* data, unsigned int size);
	virtual bool hasAvailablePackets(unsigned int* size);
	virtual bool ReadPacket(void* data, int size, unsigned int* receivedSize, unsigned  long long* userId);
	virtual bool CloseConnection(unsigned long long userId);
public:
	static OpenMultiplayer* getInstance(bool apiEnabled);
	OpenMultiplayerLocal() {
		apiEnabled = false;
	}
private:
	STEAM_CALLBACK_MANUAL(OpenMultiplayerLocal, OnSessionRequested, P2PSessionRequest_t, m_SessionRequestedCallBack);
	bool apiEnabled;

	void OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
	CCallResult< OpenMultiplayerLocal, LobbyCreated_t > m_LobbyCreatedCallResult;
	void OnLobbyEntered(LobbyEnter_t* pCallback, bool bIOFailure);
	CCallResult< OpenMultiplayerLocal, LobbyEnter_t > m_LobbyEnteredCallResult;

	unsigned long long LobbyID = 0;
};

OpenMultiplayerLocal multiplayerinstance;

OpenMultiplayer* OpenMultiplayerLocal::getInstance(bool apiEnabled)
{
	multiplayerinstance.apiEnabled = apiEnabled;
	return &multiplayerinstance;
}

OpenMultiplayer* getMultiplayerInstance(bool apiEnabled)
{
	return OpenMultiplayerLocal::getInstance(apiEnabled);
}

void OpenMultiplayerLocal::CreateLobby(int type, int maxplayers)
{
	if (apiEnabled) {
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby((ELobbyType)type, maxplayers);
		m_LobbyCreatedCallResult.Set(hSteamAPICall, this, &OpenMultiplayerLocal::OnLobbyCreated);
	}
}

int OpenMultiplayerLocal::CountLobbyUsers()
{
	if (apiEnabled && LobbyID > 0) {
		return SteamMatchmaking()->GetNumLobbyMembers(LobbyID);
	}
	return 0;
}

unsigned long long OpenMultiplayerLocal::GetLobbyUserId(int index)
{
	if (apiEnabled && LobbyID > 0) {
		return SteamMatchmaking()->GetLobbyMemberByIndex(LobbyID, index).ConvertToUint64();
	}
	return 0;
}

void OpenMultiplayerLocal::JoinLobby(unsigned long long id)
{
	if (apiEnabled) {
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(id);
		m_LobbyEnteredCallResult.Set(hSteamAPICall, this, &OpenMultiplayerLocal::OnLobbyEntered);
	}
}

void OpenMultiplayerLocal::LeaveLobby()
{
	if (apiEnabled) {
		SteamMatchmaking()->LeaveLobby(LobbyID);
		LobbyID = 0;
	}
}

bool OpenMultiplayerLocal::SendPacket(unsigned long long userId, const void* data, unsigned int size)
{
	if (apiEnabled) {
		return SteamNetworking()->SendP2PPacket(userId, data, size, k_EP2PSendUnreliable);
	}
	return false;
}

bool OpenMultiplayerLocal::hasAvailablePackets(unsigned int* size)
{
	if (apiEnabled) {
		return SteamNetworking()->IsP2PPacketAvailable(size);
	}
	return false;
}

bool OpenMultiplayerLocal::ReadPacket(void* data, int size, unsigned int* receivedSize, unsigned long long* userId)
{
	if (apiEnabled) {
		CSteamID id;
		bool result = SteamNetworking()->ReadP2PPacket(data, size, receivedSize, &id);
		*userId = id.ConvertToUint64();
		return result;
	}
	return false;
}

bool OpenMultiplayerLocal::CloseConnection(unsigned long long userId)
{
	if (apiEnabled) {
		return SteamNetworking()->CloseP2PSessionWithUser(userId);
	}
	return false;
}

void OpenMultiplayerLocal::OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
	if (bIOFailure || pCallback->m_eResult != k_EResultOK) {
		return;
	}
	LobbyID = pCallback->m_ulSteamIDLobby;
}

void OpenMultiplayerLocal::OnLobbyEntered(LobbyEnter_t* pCallback, bool bIOFailure)
{
	if (bIOFailure) {
		return;
	}
	LobbyID = pCallback->m_ulSteamIDLobby;
}

void OpenMultiplayerLocal::OnSessionRequested(P2PSessionRequest_t* pCallback)
{
	SteamNetworking()->AcceptP2PSessionWithUser(pCallback->m_steamIDRemote);
}
