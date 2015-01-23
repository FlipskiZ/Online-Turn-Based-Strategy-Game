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
#include "Building.h"

#define PI 3.14159265359
#define toRadians 0.0174532925

#define screenWidth 1280
#define screenHeight 720

#define mapDisplayWidth 1280
#define mapDisplayHeight 528

#define topGuiHeight 64
#define botGuiHeight 594//mapDisplayHeight+topGuiHeight
#define leftGuiWidth 64
#define rightGuiWidth 1280//mapDisplayWidth+leftGuiWidth

#define maxMapArrayWidth 400
#define maxMapArrayHeight 200

#define maxMineralDepth 3

#define tileSize 32

#define MAX_BUTTONS 100
#define MAX_INPUT_FIELDS 50
#define MAX_PARTICLES 1000
#define MAX_PLAYERS 8

#define FPS 60.0

#define chatLogSize 100

#define PLAYER_RED_COLOR al_map_rgb(255,0,0)
#define PLAYER_GREEN_COLOR al_map_rgb(0,255,0)
#define PLAYER_BLUE_COLOR al_map_rgb(0,0,255)
#define PLAYER_YELLOW_COLOR al_map_rgb(255,255,0)
#define PLAYER_MAGENTA_COLOR al_map_rgb(255,0,255)
#define PLAYER_CYAN_COLOR al_map_rgb(0,255,255)
#define PLAYER_BLACK_COLOR al_map_rgb(50,50,50)
#define PLAYER_WHITE_COLOR al_map_rgb(255,255,255)

#define PLAYER_RED_COLOR_TINT al_map_rgba(127,0,0,10)
#define PLAYER_GREEN_COLOR_TINT al_map_rgba(0,127,0,10)
#define PLAYER_BLUE_COLOR_TINT al_map_rgba(0,0,127,10)
#define PLAYER_YELLOW_COLOR_TINT al_map_rgba(127,127,127,10)
#define PLAYER_MAGENTA_COLOR_TINT al_map_rgba(127,0,127,10)
#define PLAYER_CYAN_COLOR_TINT al_map_rgba(0,127,127,10)
#define PLAYER_BLACK_COLOR_TINT al_map_rgba(0,0,0,10)
#define PLAYER_WHITE_COLOR_TINT al_map_rgba(127,127,127,10)

enum GameMessages{
    ID_CONNECTED_MESSAGE = ID_USER_PACKET_ENUM+1,
    ID_PLAYER_INITIALIZATION = ID_USER_PACKET_ENUM+2,
	ID_CHAT_MESSAGE = ID_USER_PACKET_ENUM+3,
	ID_TRANSFER_MAP_TILE = ID_USER_PACKET_ENUM+4,
	ID_MAP_DIMENSIONS = ID_USER_PACKET_ENUM+5,
    ID_PLACE_BUILDING = ID_USER_PACKET_ENUM+6,
    ID_END_TURN = ID_USER_PACKET_ENUM+7,
    ID_SET_RESOURCE = ID_USER_PACKET_ENUM+8,
    ID_END_TURN_SYNCHRONIZE = ID_USER_PACKET_ENUM+9,
    ID_TRANSFER_MINERAL_TILE = ID_USER_PACKET_ENUM+10,
    ID_SET_BUILDING_OWNER = ID_USER_PACKET_ENUM+11,
};

enum ResourceId{
    RESOURCE_METAL,
    RESOURCE_FOOD,
    RESOURCE_OIL,
    RESOURCE_SILVER
};

enum PlayerColorId{
    PLAYER_NONE = -1,
    PLAYER_RED,
    PLAYER_GREEN,
    PLAYER_BLUE,
    PLAYER_YELLOW,
    PLAYER_MAGENTA,
    PLAYER_CYAN,
    PLAYER_BLACK,
    PLAYER_WHITE,
};

enum BuildingTypes{
    BUILDING_CAPITAL,
    BUILDING_CONNECTOR,
    BUILDING_MINER,
    BUILDING_WEAPON,
};

enum BuildingPropertyTypes{
    BUILDING_METAL_COST,
    BUILDING_FOOD_COST,
    BUILDING_OIL_COST,
    BUILDING_SILVER_COST,
};

bool isPassable(float x, float y, float width, float height);
bool checkCollision(float x, float y, float ex, float ey, float width, float height, float ewidth, float eheight);
bool insideMap(float x, float y, float width, float height);
void addButtonToList(Button *newButton);
void addInputFieldToList(InputField *newInputField);
void addParticleToList(ParticleEntity *newParticle);
void addBuildingToList(Building *newBuilding);
void loadMapArray();
void saveMapArray();
int findBuilding(int posX, int posY);
void drawMap();
void drawTile(float x, float y, int mapOffsetX, int mapOffsetY);
void updateCamera();
bool visibleInCamera(float posX, float posY, float width, float height);
int getBuildingProperty(int buildingType, int buildingPropertyId);

extern ALLEGRO_DISPLAY *display;

extern ALLEGRO_FONT *defaultFont;
extern ALLEGRO_FONT *smallFont;
extern ALLEGRO_FONT *bigFont;

extern ALLEGRO_BITMAP *cursorImage;

extern ALLEGRO_BITMAP *groundImage1;
extern ALLEGRO_BITMAP *groundImage2;
extern ALLEGRO_BITMAP *foodResourceImage;
extern ALLEGRO_BITMAP *oilResourceImage;
extern ALLEGRO_BITMAP *metalResourceImage;
extern ALLEGRO_BITMAP *silverResourceImage;
extern ALLEGRO_BITMAP *capitalBuildingImage;
extern ALLEGRO_BITMAP *connectorBuildingImage;
extern ALLEGRO_BITMAP *minerBuildingImage;

extern ALLEGRO_KEYBOARD_STATE keyState;
extern ALLEGRO_MOUSE_STATE mouseState;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *timer;

extern Button *buttonList[MAX_BUTTONS];
extern InputField *inputFieldList[MAX_INPUT_FIELDS];
extern ParticleEntity *particleList[MAX_PARTICLES];
extern Player *playerList[MAX_PLAYERS];
extern std::vector<Building*> buildingList;

extern int mapArray[maxMapArrayWidth][maxMapArrayHeight];
extern int mapArrayRotation[maxMapArrayWidth][maxMapArrayHeight];
extern int buildingIndex[maxMapArrayWidth][maxMapArrayHeight];
extern int mineralArray[maxMapArrayWidth][maxMapArrayHeight][maxMineralDepth][2]; ///mineralArray[x][y][Depth][0 - Type --- 1 - Quantity]

extern const char* versionNumber;
extern float fpsTimeNew, fpsCounter, fpsTimeOld;

extern float cameraPosX, cameraPosY, cameraOffsetX, cameraOffsetY, mapDisplayOffsetX, mapDisplayOffsetY, cameraMoveSpeed;
extern bool drawScreen, timerEvent, done, mouseButtonLeft, mouseButtonLeftClick, mouseButtonRight, mouseButtonRightClick, inGame, allegroWrite;
extern float mouseX, mouseY;
extern int lastKeyPress, mouseWheel, mouseWheelChange;
extern int selectedBuildingId;

extern std::string allegroString;

extern int mapArrayWidth, mapArrayHeight;

extern RakNet::RakPeerInterface *rakPeer;
extern RakNet::Packet *rakPacket;
extern RakNet::SocketDescriptor rakSocketDescriptor;
extern int rakPort, rakClientId;
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
