#include <stdio.h>
#include "Server.h"
#include <iostream>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

Server::Server()
{
	room = new SlitherRoom();
}

Server::~Server()
{
}

void Server::Start()
{
	printf("Starting slither server... \n");
	//Obtain the RakPeerInterface object for managing communications
	peer = RakNet::RakPeerInterface::GetInstance();
	//create an object to store the socket which the server will run on
	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	//Start listning for connections
	peer->Startup(MAXIMUM_PLAYERS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAXIMUM_PLAYERS);
	printf("Listning for connections on port: %i \n", SERVER_PORT);
}

void Server::Process()
{
	RakNet::Packet *packet;
	//read all packets in queue
	while ((packet = peer->Receive()) != 0) {
		int packetRSDS = 0;
		switch (packet->data[0]) {
			case ID_NEW_INCOMING_CONNECTION:
				room->AddPlayer(packet->guid, packet->systemAddress);
				printf("Connection request from: %s \n", packet->systemAddress.ToString());
				break;
			case ID_CONNECTION_LOST:
			case ID_DISCONNECTION_NOTIFICATION:
				printf("Disconnected from %s \n", packet->systemAddress.ToString());
				break;
			case ID_LOGIN_REQUEST:
				room->GetPlayer(packet->guid)->getInStream()->ReceiveLoginRequest(packet->data, packet->bitSize);
				break;
			case ID_CLIENT_SEND_PLAYER_UPDATE:
				room->GetPlayer(packet->guid)->getInStream()->ReceivePlayerUpdate(packet->data, packet->bitSize);
				break;
			case ID_CLIENT_CLOSED_PROGRAM:
			{
				room->deletePlayer(packet->data, packet->bitSize);
				getPeerInterface()->CloseConnection(packet->systemAddress,true);
				printf("connected totaly : %i ",(int)getPeerInterface()->NumberOfConnections());
			}break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
		}
		peer->DeallocatePacket(packet);
	}
	room->process();
	printf("Connected totaly: %i \n", (int)room->getList().size());
}
