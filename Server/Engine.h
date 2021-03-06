#ifndef ENGINE_H
#define ENGINE_H

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>
#include <windows.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Player.h"
#include "Building.h"

#define PI 3.14159265359
#define toRadians 0.0174532925

#define maxMapArrayWidth 400
#define maxMapArrayHeight 200

#define maxMineralDepth 3

#define tileSize 32

#define MAX_PLAYERS 8

#define FPS 60.0

#define START_METAL 200
#define START_FOOD 100
#define START_OIL 200
#define START_SILVER 100

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
    ID_ATTACK_BUILDING = ID_USER_PACKET_ENUM+12,
    ID_SET_BUILDING_STATS = ID_USER_PACKET_ENUM+13,
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
    BUILDING_SMALLWEAPON,
    BUILDING_BIGWEAPON,
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
void enterCommand(std::string command);
int addPlayerToList(Player *newPlayer);
void addBuildingToList(Building *newBuilding);
void deleteBuildingFromList(int buildingId);
void loadMapArray();
void saveMapArray();
int findPlayer(RakNet::RakNetGUID senderGuid);
int findBuilding(int posX, int posY);
bool endTurn();
void connectBuildings();
int canPlaceBuilding(int buildingType, int posX, int posY, int playerId);
int getBuildingProperty(int buildingType, int buildingPropertyId);

extern Player *playerList[MAX_PLAYERS];
extern std::vector<Building*> buildingList;

extern int mapArray[maxMapArrayWidth][maxMapArrayHeight];
extern int mapArrayRotation[maxMapArrayWidth][maxMapArrayHeight];
extern int mapArrayOwner[maxMapArrayWidth][maxMapArrayHeight];
extern int buildingIndex[maxMapArrayWidth][maxMapArrayHeight];
extern int mineralArray[maxMapArrayWidth][maxMapArrayHeight][maxMineralDepth][2]; ///mineralArray[x][y][Depth][0 - Type --- 1 - Quantity]

extern RakNet::RakPeerInterface *rakPeer;
extern RakNet::Packet *rakPacket;
extern RakNet::SocketDescriptor rakSocketDescriptor;

extern const char* versionNumber;

extern bool done;
extern int port, maxConnected;
extern std::string serverCommand;

extern int mapArrayWidth, mapArrayHeight, maxPlayers;

extern bool redChosen, greenChosen, blueChosen, yellowChosen;

class GameState;

class Engine{
    public:
        void init(const char* title);
        void cleanup();

        void changeState(GameState* state);
        void pushState(GameState* state);
        void popState();

        void update();

        bool running(){ return m_running; }
        void quit(){m_running = false;}

    private:
        // the stack of states
        std::vector<GameState*> states;

        bool m_running;
};

#endif // ENGINE_H
