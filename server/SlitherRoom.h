#pragma once
#include <vector>
#include <list>
#include "Client.h"

//! The slither room class was set up with the intention of the server having multiple instances of rooms which the player can join.
class SlitherRoom {
	friend class Client;
	friend class InStream;
	friend class OutStream;
public:
	//! Add a new player instance to the room
	void AddPlayer(RakNet::RakNetGUID uuid, RakNet::SystemAddress address);
	//! Get a client instance
	Client * GetPlayer(RakNet::RakNetGUID uuid);
	//! Update the game room
	void process();

	void deletePlayer(unsigned char *data, int size);
	std::vector<Client*> getList() { return players; }
private:
	std::vector<Client*> players;
};