#include "Stream.h"
#include "Server.h"
#include <BitStream.h>
#include <string>

extern Server * s;
using namespace RakNet;
void InStream::ReceiveLoginRequest(unsigned char * data, int size)
{
	RakNet::BitStream bitStream(data, size, false);
	RakNet::RakString name;
	bitStream.IgnoreBytes(sizeof(RakNet::MessageID));
	bitStream.Read(name);
	p->username = name;
	int response = 0; //accept login reuqest
	//send response
	p->getOutStream()->SendLoginResponse(response, RakNet::RakNetGUID::ToUint32(p->uuid), name.C_String());
	//Hint: Send an update to all players in the game to tell them about this new client

}

void InStream::ReceivePlayerUpdate(unsigned char * data, int size)
{
	float angle;
	int mouseDown;
	RakNet::BitStream bitStream(data, size, false);
	bitStream.IgnoreBytes(sizeof(RakNet::MessageID));
	bitStream.Read(angle);
	bitStream.Read(mouseDown);
	p->SetHeadAngle(angle);
}

void OutStream::SendLoginResponse(int result, unsigned long playerId, std::string name)
{
	//initial player settings
	float angle = 0;
	int length = 8;
	double positionX = 128;
	double positionY = 128;
	RakNet::RakString RSname = name.c_str();
	RakNet::BitStream bitStream;
	bitStream.Write((RakNet::MessageID) ID_LOGIN_RESPONSE);
	bitStream.Write(result);
	bitStream.Write(playerId);
	bitStream.Write(RSname);
	bitStream.Write(angle);
	bitStream.Write(length);
	bitStream.Write(positionX);
	bitStream.Write(positionY);
	
#if _DEBUG
#endif
	s->getPeerInterface()->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->getUUID(), false);
	//..set info to all other players that the other player is connected
	for (int i = 0;  i < s->room->players.size(); i++)
	{
		s->room->players[i]->getOutStream()->SendAddPlayer(RakNetGUID::ToUint32(
			s->room->players[i]->getUUID()),
			s->room->players[i]->getUsername(),
			s->room->players[i]->position,
			s->room->players[i]->bodyPositions);
		
	}
	p->getOutStream()->SendAddPlayer(RakNetGUID::ToUint32(p->uuid), p->getUsername(), p->position, p->bodyPositions);
}

void OutStream::SendPlayerUpdate(unsigned long uuid, double x, double y, float headAngle)
{

		//build bitstream on player (n)
		BitStream bs;
		bs.Write((RakNet::MessageID) ID_SERVER_SEND_PLAYER_UPDATE);
		bs.Write(uuid);
		bs.Write(x);
		bs.Write(y);
		bs.Write(headAngle);

		for (auto &c : s->room->players)
		{
			s->getPeerInterface()->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,c->getUUID(), false);
			//send the message to other players (m)
		}
	//Hint: Send a packet to the client here
}

void OutStream::SendRemovePlayer(unsigned long uuid)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID) ID_DELETE_PLAYER);
	bs.Write(uuid);

	for (auto &c : s->room->players)
	{

			s->getPeerInterface()->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, c->getUUID(), false);
		
	}
}
// server stream.cpp
void OutStream::SendAddPlayer(unsigned long uuid, std::string name, Vector2<double> position, std::vector<Vector2<double>> bodyPositions)
{
	BitStream bs;
	RakString rs = name.c_str();
	bs.Write((RakNet::MessageID) ID_PLAYER_ADD);
	bs.Write(uuid);
	bs.Write(rs);
	bs.Write(position.x);
	bs.Write(position.y);
	bs.Write(bodyPositions.size());

	for (int i = 0; i < bodyPositions.size(); i++) {
		bs.Write(bodyPositions[i].x);
		bs.Write(bodyPositions[i].y);
	}
	for (auto &c : s->room->players)
	{
		s->getPeerInterface()->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, c->getUUID(), false);
	}
}
	
