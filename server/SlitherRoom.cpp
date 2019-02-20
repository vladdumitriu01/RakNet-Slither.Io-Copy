#include "SlitherRoom.h"

void SlitherRoom::AddPlayer(RakNet::RakNetGUID uuid, RakNet::SystemAddress address)
{
	Client * p = new Client(128, 128, uuid, address);
	players.push_back(p);
}

Client * SlitherRoom::GetPlayer(RakNet::RakNetGUID uuid)
{
	for (auto &p : players) {
		if (p->getUUID() == uuid) {
			return p;
		}
	}
	return nullptr;
}

void SlitherRoom::process()
{
	for (auto& p : players) {
		p->process();
	}
}

void SlitherRoom::deletePlayer(unsigned char *data, int size)
{
	RakNet::BitStream bs(data, size, false);
	unsigned long uuid;
	bs.IgnoreBytes(sizeof(RakNet::MessageID));
	bs.Read(uuid);
	int index = 0;
	for (auto &p : players) {
		if (RakNet::RakNetGUID::ToUint32( p-> getUUID()) == uuid) {
			printf("player: %s -  exists the program! \n", p->getUsername().c_str());
			//p->getOutStream()->SendRemovePlayer(uuid);
			players.erase(players.begin() + index);

			
		}
		index++;
	}
	
}
