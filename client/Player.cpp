#include "Player.h"
#include "Client.h"

extern Client * c;

Player::Player(unsigned long uuid,std::string name, int x, int y) {
	this->position.x = x;
	this->position.y = y;
	this->uuid = uuid;
	this->name = name;
}

Player::~Player() {

}

void Player::Update()
{
	double oldHeadX = position.x;
	double oldHeadY = position.y;

	float fX = sin(DegToRad(headAngle));
	float fY = cos(DegToRad(headAngle));
	position.x += (fX * GetMoveSpeed()) * c->GetDeltaTime();
	position.y -= (fY * GetMoveSpeed()) * c->GetDeltaTime();

	//update body
	if (GetTime() - bodyUpdate >= 200) {
		for (int i = bodyPositions.size() - 1; i >= 1; i--) {
			bodyPositions[i].x = bodyPositions[i - 1].x;
			bodyPositions[i].y = bodyPositions[i - 1].y;
		}

		bodyPositions[0].x = oldHeadX;
		bodyPositions[0].y = oldHeadY;
		bodyUpdate = GetTime();
	}
}

void Player::Draw()
{
	//draw name
	c->DrawString(3, name, position.x - c->getGameScreen()->cameraPosition.x, (position.y + 35) - c->getGameScreen()->cameraPosition.y);
	//draw body
	for (auto & a : bodyPositions) {
		c->DrawSprite(5, a.x - c->getGameScreen()->cameraPosition.x, a.y - c->getGameScreen()->cameraPosition.y);
	}
	//draw head
	c->DrawRotatedSprite(4, position.x - c->getGameScreen()->cameraPosition.x, position.y - c->getGameScreen()->cameraPosition.y, (int)headAngle, 0, 0);
	
}
