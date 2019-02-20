#pragma once
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_Image.h>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include "MathUtil.h"
#include "LoginScreen.h"
#include "GameScreen.h"
#include "Stream.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDRESS "127.0.0.1¦3194" //edit to change the server IP address. 127.0.0.1 will connect to localhost
#define SERVER_PORT 3194 //can be set to any port, but must match server

#define TOTAL_SPRITES 8 //8 total sprites loaded
#define TOTAL_FONTS 4 //4 total fonts (defined in fontdef.dat)

//forward declarations
class LoginScreen;
class GameScreen;

//!class Client represents the game window as well as network in and out streams
class Client {
	//provide access to private data for instream and outstream
	friend class InStream;
	friend class OutStream;
public:
	//Default
	Client();
	//Deconstructor
	virtual ~Client();

	//!Initialise all relevant SDL libraries and creates a window
	void CreateSDLWindow();
	//!Loads all game assets
	void LoadAssets();
	//!Update the game logic
	bool Update();
	//!Draw the game
	void Draw();
	//!Remove all loaded assets from memory - called in deconstructor
	void UnloadAssets();

	//!shortcut function to draw text to window
	void DrawString(int fontId, std::string text, int x = 0, int y = 0, SDL_Color textColor = { 255,255,255 });
	//!shortcut to draw sprite to window
	void DrawSprite(int spriteId, int x = 0, int y = 0);
	//!shortcut to draw sprite to window with rotation
	void DrawRotatedSprite(int spriteId, int x, int y, int angle, int halfWidth, int halfHeight);

	//!Set the frame deltat time - Should only be called in ClientApplication.cpp
	void SetDeltaTime(float f) { frameDeltaTime = f; }
	//!return a float which represents the time between frames. i.e. 0.5 would mean the game takes half a second to render.
	float GetDeltaTime() { return frameDeltaTime; }

	//!return the mouse X position
	int getMouseX() { return mouseX; }
	//!return the mouse Y position
	int getMouseY() { return mouseY; }
	//!return 1 if the mouse is pressed down 0 for not pressed
	int getMouseDown() { return mouseDown; }
	//!returns true for one frame if the last state was down and the current state is up
	int getMousePressed() { return !mouseDown && lastMousePressedState; }

	//! returns the with of any sprite
	int SpriteWidth(int spriteId) { return GetSprite(spriteId)->w; }
	//! returns the height of any sprite
	int SpriteHeight(int spriteId) { return GetSprite(spriteId)->h; }

	//! gets the raknet peer RakPeerInterface object
	RakNet::RakPeerInterface * getPeerInterface() { return peer; }
	//! gets the server address
	RakNet::SystemAddress getServerAddress() { return serverAddress; }

	//! returns a class which has functions for receiving data from the server
	InStream getInStream() { return inStream; }
	//! returns a class which has functions for sending data to the server
	OutStream getOutStream() { return outStream; }

	//! Sets the current screen of the client. i.e. 0 = loading, 1 = game
	void EnterGameScreen(int screen) { gameState = screen; }

	//! Returns the game screen which contains spesific data about the game environment
	GameScreen * getGameScreen() const { return gameScreen; }

	void exitProgram();
private:
	float frameDeltaTime;
	int gameState = 0;
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Surface ** sprites;
	SDL_Texture ** textures;
	TTF_Font ** fonts;
	LoginScreen * loginScreen;
	GameScreen * gameScreen;
	SDL_Surface * GetSprite(int index);
	TTF_Font * GetFont(int index);
	SDL_Texture * GetTexture(int index);
	int mouseX, mouseY;
	int lastMousePressedState = 0;
	int mouseDown = 0;
	RakNet::RakPeerInterface * peer;
	RakNet::SystemAddress serverAddress;
	std::string username;
	InStream inStream;
	OutStream outStream;
};