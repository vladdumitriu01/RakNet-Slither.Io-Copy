#pragma once
#include "MathUtil.h"
#include <vector>

class Player {
	friend class GameScreen;
	friend class InStream;
	friend class OutStream;
public:
	//constructor
	/**
	* \param uuid - the unique identifer to identify the player when server messages are received
	* \param name - the username of the player
	* \param x - the starting X position
	* \param y - the starting Y position
	*/
	Player(unsigned long uuid, std::string name, int x, int y);
	//destructor
	virtual ~Player();
	//! Returns the movement speed of the player
	inline float GetMoveSpeed() const { return 20.0f; }
	//! Updates the player
	void Update();
	//! Draw the player
	void Draw();

	unsigned long getUUID() { return uuid; }

protected:
	std::string name;
	unsigned long uuid;
	float headAngle = 0; //direction the player is travelling
	int headRadius = 23;
	int length = 8; //length of the snake
	int boost = 0;
	Vector2<double> position;//position on the map
	std::vector<Vector2<double>> bodyPositions;
	long bodyUpdate;
};