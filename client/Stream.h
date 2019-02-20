#pragma once
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <string>

//! All Game Message IDs
//! This should be the same in server & client
enum GAME_MESSAGES {
	ID_LOGIN_REQUEST = ID_USER_PACKET_ENUM + 1,
	ID_LOGIN_RESPONSE,
	ID_ORB_POSITIONS_ADD,
	ID_ORB_POSITIONS_REMOVE,
	ID_PLAYER_ADD,
	ID_CLIENT_SEND_PLAYER_UPDATE,
	ID_SERVER_SEND_PLAYER_UPDATE,
	ID_CLIENT_CLOSED_PROGRAM,
	ID_DELETE_PLAYER

};


//! All outgoing messages to the server
class OutStream {
public:
	//! Sends a login message to the server
	/** 
	 * \param userName - pass the name of the user has entered in the login box
	 */
	void SendLoginRequest(std::string userName);
	//! Sends an update to the server of the players input
	/**
	* \param angle - the angle of the players head (this is calculated with the mouse position)
	* \param mouseDown - (not supported) player can press mouse down to boost snake
	*/
	void SendPlayerUpdate(float angle, int mouseDown);
	void SendMessageClosed();
};

//! All incoming messages from the server
class InStream {
public:
	//! Receive a login response from the server
	/**
	* \param data - the payload of the packet
	* \param size - the size in bytes of the packet
	*/
	void ReceiveLoginResponse(unsigned char * data, unsigned int size);
	//! Receive an update for a player
	/**
	* \param data - the payload of the packet
	* \param size - the size in bytes of the packet
	*/
	void ReceivePlayerUpdate(unsigned char * data, unsigned int size);
	//! Add or remove a player
	/**
	* \param data - the payload of the packet
	* \param size - the size in bytes of the packet
	*/
	void ReceivePlayerAdd(unsigned char * data, unsigned int size);
};
