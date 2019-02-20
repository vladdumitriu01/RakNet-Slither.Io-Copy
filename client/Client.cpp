#include <stdio.h>
#include "Client.h"
using namespace RakNet;
Client::Client()
{
	loginScreen = new LoginScreen();
	gameScreen = new GameScreen();
	peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd;
	peer->Startup(1, &sd, 1);
}

Client::~Client()
{
	UnloadAssets();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SAFERELEASE(loginScreen);
	SAFERELEASE(gameScreen);
}

void Client::CreateSDLWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Error initialising SDL: %s \n", SDL_GetError());
		return;
	}
	
	
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}
	

	window = SDL_CreateWindow("Slither Game Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		printf("Error creating window: %s \n", SDL_GetError());
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		printf("Failed to create renderer.");
	}
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return;
	}

	if (TTF_Init() < 0) {
		printf("TTF could not be initialised!", SDL_GetError());
	}
}

void Client::LoadAssets()
{
	sprites = new SDL_Surface*[TOTAL_SPRITES];
	textures = new SDL_Texture*[TOTAL_SPRITES];
	fonts = new TTF_Font*[TOTAL_FONTS];
	struct Font { std::string font = ""; int size; };
	Font fontData[TOTAL_FONTS];
	std::ifstream file("assets/fonts/fontdef.dat");
	if (file.is_open()) {
		std::string line;
		int i = 0;
		while (file >> fontData[i].font >> fontData[i].size) {
			printf("Font type: %s size: %i \n", fontData[i].font, fontData[i].size);
			i++;
			if (i >= TOTAL_FONTS) {
				break;
			}
		}
	}
	else {
		printf("Failed to load font definitions");
	}

	for (int i = 0; i < TOTAL_SPRITES; i++) {
		std::stringstream filePath;
		filePath << "assets/sprites/";
		filePath << i;
		filePath << ".png";
		sprites[i] = IMG_Load(filePath.str().c_str());
		if (sprites[i] != NULL) {
			SDL_SetColorKey(sprites[i], SDL_TRUE, SDL_MapRGB(sprites[i]->format, 0, 0xFF, 0xFF));
		}
		textures[i] = SDL_CreateTextureFromSurface(renderer, sprites[i]);
		
		if (!textures[i]) 
		printf("Unable to load sprite %i! SDL Error: %s\n", i, SDL_GetError());
	}

	for (int i = 0; i < TOTAL_FONTS; i++) {
		if (fontData[i].font != "") {
			std::stringstream filePath;
			filePath << "assets/fonts/";
			filePath << fontData[i].font;
			filePath << ".ttf";
			fonts[i] = TTF_OpenFont(filePath.str().c_str(), fontData[i].size);
		}
		if (fonts[i] == NULL) {
			printf("Unable to load font %i! SDL Error: %s\n", i, SDL_GetError());
		}
	}
}

bool Client::Update()
{

	//look for connection request to be accepted
	RakNet::Packet * p;
	while ((p = getPeerInterface()->Receive()) != 0) {
		switch (p->data[0]) {
		case ID_CONNECTION_REQUEST_ACCEPTED:
			getOutStream().SendLoginRequest(loginScreen->usernameText);
			EnterGameScreen(1);
			break;
		case ID_LOGIN_RESPONSE:
			getInStream().ReceiveLoginResponse(p->data, p->length);
			break;
		case ID_ORB_POSITIONS_ADD:
			//			getInStream().RecieveAddOrbPositions(p->data, p->length);
			break;
		case ID_SERVER_SEND_PLAYER_UPDATE:
			getInStream().ReceivePlayerUpdate(p->data, p->length);
			break;
		case ID_PLAYER_ADD:
			getInStream().ReceivePlayerAdd(p->data, p->length);
			break;
		
		 case ID_DELETE_PLAYER:
		 {
			 getGameScreen()->removePlayer(p->data, p->bitSize);
		 }break;
	}
		getPeerInterface()->DeallocatePacket(p);
	}


	SDL_Event e;
	lastMousePressedState = mouseDown;

	while (SDL_PollEvent(&e) != 0) {

		if (e.type == SDL_QUIT) {
			return true;
		}
		
		if (e.type == SDL_MOUSEMOTION) {
			mouseX = e.motion.x;
			mouseY = e.motion.y;
		}

		if (e.type == SDL_MOUSEBUTTONDOWN) {
			mouseDown = true;
		}

		if (e.type == SDL_MOUSEBUTTONUP) {
			mouseDown = false;
		}

		if (e.type == SDL_KEYDOWN) {
			switch (gameState) {
			case 0:
				loginScreen->KeyDown(e.key.keysym.sym);
				break;
			case 1:
				gameScreen->KeyDown(e.key.keysym.sym);
				break;
			}
			
		}
	}

	switch (gameState) {
	case 0:
		return loginScreen->Update();
		break;
	case 1:
		return gameScreen->Update();
		break;
	}
	return false;
}

void Client::Draw()
{
	//clear screen to grey (background color)
	//SDL_FillRect(renderFrame, NULL, 0x161c22);
	SDL_SetRenderDrawColor(renderer, 0x16, 0x1c, 0x22, 0xff);
	SDL_RenderClear(renderer);

	switch (gameState) {
	case 0:
		loginScreen->Draw();
		break;
	case 1:
		gameScreen->Draw();
		break;
	}

	//SDL_UpdateWindowSurface(window);
	SDL_RenderPresent(renderer);
}

void Client::UnloadAssets()
{
	for (int i = 0; i < TOTAL_SPRITES; i++) {
		SDL_FreeSurface(sprites[i]);
	}
	delete[] sprites;
	for (int i = 0; i < TOTAL_FONTS; i++) {
		TTF_CloseFont(fonts[i]);
	}
	delete[] fonts;
}

void Client::DrawString(int fontId, std::string text, int x, int y, SDL_Color textColor)
{
	SDL_Surface* textSurface = TTF_RenderText_Solid(GetFont(fontId), text.c_str(), textColor);
	if (textSurface != NULL) {
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = textSurface->w;
		rect.h = textSurface->h;
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

		//SDL_BlitSurface(textSurface, NULL, renderFrame, &rect);
		SDL_RenderCopy(renderer, textTexture, NULL, &rect);
		SDL_DestroyTexture(textTexture);
		SDL_FreeSurface(textSurface);
	}
}

void Client::DrawSprite(int spriteId, int x, int y)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = GetSprite(spriteId)->w;
	rect.h = GetSprite(spriteId)->h;
	SDL_RenderCopyEx(renderer, GetTexture(spriteId), NULL, &rect, 0, NULL, SDL_RendererFlip::SDL_FLIP_NONE);
	//SDL_BlitSurface(GetSprite(spriteId), NULL, renderFrame, &rect);
}

void Client::DrawRotatedSprite(int spriteId, int x, int y, int angle, int w, int h)
{
	SDL_Rect rect = { x, y, GetSprite(spriteId)->w, GetSprite(spriteId)->h };
	SDL_Rect clip = { x, y, w, h };
	SDL_Texture* texture = GetTexture(spriteId);
	SDL_RenderCopyEx(renderer, texture, NULL, &rect, angle, NULL, SDL_RendererFlip::SDL_FLIP_NONE);
}

void Client::exitProgram()
{
	BitStream bs;
	bs.Write((MessageID)ID_CLIENT_CLOSED_PROGRAM);
	bs.Write(getGameScreen()->getMyself()->getUUID());
	getPeerInterface()->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, getServerAddress(), true);
}

SDL_Surface * Client::GetSprite(int index)
{
	if (index < 0 || index >= TOTAL_SPRITES) {
		return NULL;
	}
	return sprites[index];
}

TTF_Font * Client::GetFont(int index)
{
	if (index < 0 || index >= TOTAL_FONTS) {
		return NULL;
	}
	return fonts[index];
}

SDL_Texture * Client::GetTexture(int index)
{
	if (index < 0 || index >= TOTAL_SPRITES) {
		return NULL;
	}
	return textures[index];
}
