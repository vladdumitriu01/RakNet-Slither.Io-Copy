#include "Stream.h"
#include "Client.h"

extern Client * c;
	
void OutStream::SendLoginRequest(std::string userName)
{
	RakNet::BitStream bitStream;
	bitStream.Write((RakNet::MessageID) ID_LOGIN_REQUEST);
	bitStream.Write(userName.c_str());
	c->getPeerInterface()->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, c->getServerAddress(), true);
}

void OutStream::SendPlayerUpdate(float angle, int mouseDown)
{
	RakNet::BitStream bitStream;
	bitStream.Write((RakNet::MessageID) ID_CLIENT_SEND_PLAYER_UPDATE);
	bitStream.Write(angle);
	bitStream.Write(mouseDown);
//#if _DEBUG
//	printf("Sending Player Update - angle: %f mouse: %i \n", angle, mouseDown);
//#endif
	c->getPeerInterface()->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, c->getServerAddress(), true);
}

void OutStream::SendMessageClosed()
{
	RakNet::BitStream bs;

	bs.Write((RakNet::MessageID)ID_CLIENT_CLOSED_PROGRAM);
	bs.Write(c->getGameScreen()->mainPlayer->getUUID());
}

void InStream::ReceiveLoginResponse(unsigned char * data, unsigned int size)
{
	RakNet::BitStream bitStream(data, size, false);
	int packetId;
	unsigned long playerId;
	int response;
	float angle;
	int length;
	double x, y;
	RakNet::RakString name;
	bitStream.IgnoreBytes(sizeof(RakNet::MessageID));
	bitStream.Read(response);
	bitStream.Read(playerId);
	bitStream.Read(name);
	bitStream.Read(angle);
	bitStream.Read(length);
	bitStream.Read(x);
	bitStream.Read(y);
	std::vector<Vector2<double>> bodyPositions;
	for (int i = 0; i < length; i++) {
		bodyPositions.push_back(Vector2<double>(x, y + (16 * i)));
	}

	c->getGameScreen()->AddPlayer(true, playerId, name.C_String(), x, y, bodyPositions);
	int i;
}

void InStream::ReceivePlayerUpdate(unsigned char * data, unsigned int size)
{
	unsigned long playerId;
	double x, y;
	float headAngle;
	RakNet::RakString name;
	RakNet::BitStream bitStream(data, size, false);
	bitStream.IgnoreBytes(sizeof(RakNet::MessageID));
	bitStream.Read(playerId);
	bitStream.Read(x);
	bitStream.Read(y);
	bitStream.Read(headAngle);
	unsigned long mainPlayerId = c->getGameScreen()->mainPlayer->uuid;
	if (playerId == mainPlayerId) {
		c->getGameScreen()->mainPlayer->position.x = x;
		c->getGameScreen()->mainPlayer->position.y = y;
		c->getGameScreen()->mainPlayer->headAngle = headAngle;
	}
	else {
		Player * p = c->getGameScreen()->getPlayer(playerId);
		if (p != nullptr) {
			p->position.x = x;
			p->position.y = y;
			p->headAngle = headAngle;
		}
	}
}
//client stream.cpp
void InStream::ReceivePlayerAdd(unsigned char * data, unsigned int size)
{
	unsigned long uuid;
	RakNet::RakString name;
	double x, y;
	int bodySize;
	std::vector<Vector2<double>> bodyPositions;
	RakNet::BitStream bitStream(data, size, false);
	bitStream.IgnoreBytes(sizeof(RakNet::MessageID));
	bitStream.Read(uuid);
	bitStream.Read(name);
	bitStream.Read(x);
	bitStream.Read(y);
	bitStream.Read(bodySize);
	for (int i = 0; i < bodySize; i++) {
		double xx, yy;
		bitStream.Read(xx);
		bitStream.Read(yy);
		bodyPositions.push_back(Vector2<double>(xx, yy));
	}

	c->getGameScreen()->AddPlayer(false, uuid, name.C_String(), x, y, bodyPositions);
}
