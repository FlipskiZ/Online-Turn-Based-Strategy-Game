#ifndef ENGINE_H
#define ENGINE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>
#include <string>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <fstream>

#include "Button.h"
#include "Player.h"
#include "ParticleEntity.h"
#include "InputField.h"

#define PI 3.14159265359
#define toRadians 0.0174532925

#define screenWidth 1280
#define screenHeight 720

#define mapDisplayWidth 1280
#define mapDisplayHeight 560

#define topGuiHeight 32
#define botGuiHeight 592//mapDisplayHeight+topGuiHeight

#define maxMapArrayWidth 400
#define maxMapArrayHeight 200

#define tileSize 32

#define MAX_BUTTONS 100
#define MAX_INPUT_FIELDS 50
#define MAX_PARTICLES 1000
#define MAX_PLAYERS 1

#define FPS 60.0

#define chatLogSize 100

enum GameMessages{
    ID_CONNECTED_MESSAGE = ID_USER_PACKET_ENUM+1,
    ID_PLAYER_INITIALIZATION = ID_USER_PACKET_ENUM+2,
	ID_CHAT_MESSAGE = ID_USER_PACKET_ENUM+3,
	ID_TRANSFER_MAP_TILE = ID_USER_PACKET_ENUM+4,
	ID_MAP_DIMENSIONS = ID_USER_PACKET_ENUM+5,
};

bool isPassable(float x, float y, float width, float height);
bool checkCollision(float x, float y, float ex, float ey, float width, float height, float ewidth, float eheight);
bool insideMap(float x, float y, float width, float height);
void addButtonToList(Button *newButton);
void addInputFieldToList(InputField *newInputField);
void addParticleToList(ParticleEntity *newParticle);
void loadMapArray();
void saveMapArray();
void drawMap();
void drawTile(float x, float y, int tileId);
void updateCamera();

extern ALLEGRO_DISPLAY *display;

extern ALLEGRO_FONT *defaultFont;
extern ALLEGRO_FONT *smallFont;
extern ALLEGRO_FONT *bigFont;

extern ALLEGRO_BITMAP *cursorImage;
extern ALLEGRO_BITMAP *explosionImage;

extern ALLEGRO_BITMAP *groundImage1;
extern ALLEGRO_BITMAP *groundImage2;
extern ALLEGRO_BITMAP *wallImage;
extern ALLEGRO_BITMAP *brokenWallImage;
extern ALLEGRO_BITMAP *floorImage;
extern ALLEGRO_BITMAP *brokenFloorImage;
extern ALLEGRO_BITMAP *bloodGroundImage1;
extern ALLEGRO_BITMAP *bloodGroundImage2;
extern ALLEGRO_BITMAP *vineWallImage;
extern ALLEGRO_BITMAP *foodResourceImage;
extern ALLEGRO_BITMAP *oilResourceImage;
extern ALLEGRO_BITMAP *metalResourceImage;
extern ALLEGRO_BITMAP *silverResourceImage;

extern ALLEGRO_KEYBOARD_STATE keyState;
extern ALLEGRO_MOUSE_STATE mouseState;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *timer;

extern Button *buttonList[MAX_BUTTONS];
extern InputField *inputFieldList[MAX_INPUT_FIELDS];
extern ParticleEntity *particleList[MAX_PARTICLES];
extern Player *playerList[MAX_PLAYERS];

extern int mapArray[maxMapArrayWidth][maxMapArrayHeight];
extern int mapArrayRotation[maxMapArrayWidth][maxMapArrayHeight];

extern const char* versionNumber;
extern float fpsTimeNew, fpsCounter, fpsTimeOld;

extern float cameraPosX, cameraPosY, cameraOffsetX, cameraOffsetY, mapDisplayOffsetX, mapDisplayOffsetY, cameraMoveSpeed;
extern bool drawScreen, timerEvent, done, mouseButtonLeft, mouseButtonLeftClick, mouseButtonRight, mouseButtonRightClick, inGame, allegroWrite;
extern float mouseX, mouseY;
extern int lastKeyPress, mouseWheel, mouseWheelChange;

extern std::string allegroString;

extern int mapArrayWidth, mapArrayHeight;

extern RakNet::RakPeerInterface *rakPeer;
extern RakNet::Packet *rakPacket;
extern RakNet::SocketDescriptor rakSocketDescriptor;
extern int rakPort;
extern bool lanMode;
extern RakNet::SystemAddress serverAddress;
extern std::string rakIpAdress, rakClientName, serverIdSearch;

extern std::string chatLog[chatLogSize];

class GameState;

class Engine{
    public:
        void init(const char* title, int width, int height, bool fullscreen);
        void cleanup();

        void changeState(GameState* state);
        void pushState(GameState* state);
        void popState();

        void update();
        void draw();

        bool running(){ return m_running; }
        void quit(){m_running = false;}

    private:
        // the stack of states
        std::vector<GameState*> states;

        bool m_running;
        bool m_fullscreen;
};

#endif // ENGINE_H
