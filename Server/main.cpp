#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

//Initalization +
struct TILE_TYPE{
	bool isPassable;
};

bool isPassable(float x, float y, float width, float height, float deltaX, float deltaY);
bool checkCollision(float x, float y, float ex, float ey, float width, float height, float ewidth, float eheight);
bool insideMap(float x, float y, float width, float height);
void enterCommand(std::string command);
int addPlayerToList(Player *newPlayer);
void addBuildingToList(Building *newBuilding);
void loadMapArray();
void saveMapArray();
int findPlayer(RakNet::RakNetGUID senderGuid);
int findBuilding(int posX, int posY);
bool endTurn();
void connectBuildings();

Player *playerList[MAX_PLAYERS];
std::vector<Building*> buildingList;

RakNet::RakPeerInterface *rakPeer;
RakNet::Packet *rakPacket;
RakNet::SocketDescriptor rakSocketDescriptor;

Engine engine;

int mapArray[maxMapArrayWidth][maxMapArrayHeight]; ///Multi-dimensional mapArray[x][y]
int mapArrayRotation[maxMapArrayWidth][maxMapArrayHeight];
int mapArrayOwner[maxMapArrayWidth][maxMapArrayHeight];
int buildingIndex[maxMapArrayWidth][maxMapArrayHeight];
int mineralArray[maxMapArrayWidth][maxMapArrayHeight][maxMineralDepth][2]; ///mineralArray[x][y][Depth][0 - Type --- 1 - Quantity]

const char* versionNumber;

bool done;
int port;
std::string serverCommand;

int mapArrayWidth, mapArrayHeight, maxPlayers;

bool redChosen, greenChosen, blueChosen, yellowChosen;

TILE_TYPE tileIndex[] = {
	{true}, // (0) TILE_GROUND1
	{true}, // (1) TILE_GROUND2
	{false}, // (2) TILE_WALL
	{false}, // (3) TILE_BROKENWALL
	{false}, // (4) TILE_PORTAL1
	{false}, // (5) TILE_PORTAL2
	{true}, // (6) TILE_FLOOR
	{true}, // (7) TILE_BROKENFLOOR
	{true}, // (8) TILE_BLOODGROUND1
	{true}, // (9) TILE_BLOODGROUND2
	{false}, // (10) TILE_VINEWALL
};
//Initalization -

int main(){
    srand(time(0));

	Engine engine;

	//Initialize the Engine
	engine.init("Allegro 5 & RakNet 2D Game Engine 7-1-2015 - Server");

	//Load the Menu
	engine.changeState(PlayState::instance());

    std::cout << "Which port to use? The default is 2020." << std::endl;
    std::cin >> port;

    std::cout << std::endl << "What should be the max amount of players on the server? Max: 8" << std::endl;
    std::cin >> maxPlayers;

    if(maxPlayers > 8)
        maxPlayers = 8;

    std::cout << std::endl;

    rakPeer = RakNet::RakPeerInterface::GetInstance();
    RakNet::SocketDescriptor rakSocketDescriptorTemp(port, 0);
    rakSocketDescriptor = rakSocketDescriptorTemp;
    rakPeer->Startup(maxPlayers, &rakSocketDescriptor, 1);
    rakPeer->SetMaximumIncomingConnections(maxPlayers);

	//Main Loop
    while(engine.running()){
        Sleep(1000/FPS);
        engine.update(); //Call the gameState specfic update
    }
	// cleanup the engine
	engine.cleanup();
}

bool isPassable(float x, float y, float width, float height){
    if(!insideMap(x, y, width, height)){
        return false;
    }else{
        int tX = floor(x/tileSize), tY = floor(y/tileSize), tWX = floor((x+width-1)/tileSize), tHY = floor((y+height-1)/tileSize);

        if(!tileIndex[mapArray[tX][tY]].isPassable || !tileIndex[mapArray[tWX][tY]].isPassable || !tileIndex[mapArray[tX][tHY]].isPassable || !tileIndex[mapArray[tWX][tHY]].isPassable){
            return false;
        }
    }
    return true;
}

bool checkCollision(float x, float y, float ex, float ey, float width, float height, float ewidth, float eheight){
    if(x + width - 1 < ex || x > ewidth + ex - 1 || y + height - 1 < ey || y > eheight + ey - 1){
        return false;
    }else{
        return true;
    }
}

bool insideMap(float x, float y, float width, float height){
    if(x < 0 || x + width-1 >= mapArrayWidth*tileSize || y < 0 || y + height-1 >= mapArrayHeight*tileSize){
        return false;
    }
    return true;
}

void enterCommand(std::string command){
    printf("Entered command: %s\n", command.c_str());
    if(command.compare("shutdown") || command.compare("exit")){
        rakPeer->Shutdown(300);
        engine.quit();
    }else if(command.compare("print")){
        printf("\nX:%d Y:%d\n", buildingList[0]->getBuildingPosX(), buildingList[0]->getBuildingPosY());
    }
}

int addPlayerToList(Player *newPlayer){
    for(int i = 0; i < maxPlayers; i++){
        if(playerList[i] == NULL){
            newPlayer->setPlayerId(i);
            playerList[i] = newPlayer;
            return i;
        }
    }
    return -1;
}

void addBuildingToList(Building *newBuilding){
    buildingIndex[newBuilding->getBuildingPosX()][newBuilding->getBuildingPosY()] = buildingList.size();
    newBuilding->setBuildingId(buildingList.size());
    buildingList.push_back(newBuilding);
}

void loadMapArray(){
    std::ifstream mapArrayFile;
    mapArrayFile.open("config/MapArray.txt");

    for(int y = 0; y < mapArrayHeight; y++){
        for(int x = 0; x < mapArrayWidth; x++){
            mapArrayFile >> mapArray[x][y];
        }
    }

    mapArrayFile.close();
}

void saveMapArray(){
    std::ofstream mapArrayFile;
    mapArrayFile.open("config/MapArray.txt");

    for(int y = 0; y < mapArrayHeight; y++){
        for(int x = 0; x <= mapArrayWidth; x++){
            if(x < mapArrayWidth){
                mapArrayFile << mapArray[x][y] << " ";
            }else if(x == mapArrayWidth){
                mapArrayFile << std::endl;
            }
        }
    }

    mapArrayFile.close();
}

int findPlayer(RakNet::RakNetGUID senderGuid){
    for(int i = 0; i < maxPlayers; i++){
        if(playerList[i] != NULL && strcmp(playerList[i]->playerGuid.ToString(), senderGuid.ToString()) == 0){
            return i;
        }
    }
    return -1;
}

int findBuilding(int posX, int posY){
    if(posX > 0 && posX < mapArrayWidth && posY > 0 && posY < mapArrayHeight)
        return buildingIndex[posX][posY];
    return -1;
}

bool endTurn(){
    for(int i = 0; i < maxPlayers; i++){
        if(playerList[i] != NULL){
            if(!playerList[i]->getPlayerTurn()){
                return false;
            }
        }
    }

    connectBuildings();

    for(int i = 0; i < maxPlayers; i++){
        if(playerList[i] != NULL){
            playerList[i]->endPlayerTurn();
        }
    }
    return true;
}

void connectBuildings(){
    int mapArrayOwnerTemp[mapArrayWidth][mapArrayHeight];
    for(int x = 0; x < mapArrayWidth; x++){
        for(int y = 0; y < mapArrayHeight; y++){
            mapArrayOwnerTemp[x][y] = mapArrayOwner[x][y];
            mapArrayOwner[x][y] = -1;
        }
    }
    for(int i = 0; i < buildingList.size(); i++){
        if(!buildingList[i]->getBuildingCapital()){
            buildingList[i]->setBuildingOwner(-1);
        }
    }
    for(int i = 0; i < buildingList.size(); i++){
        if(buildingList[i]->getBuildingCapital()){
            buildingList[i]->checkConnectedNeighbours();
        }
    }
    RakNet::BitStream bitStreamOUT;
    for(int x = 0; x < mapArrayWidth; x++){
        for(int y = 0; y < mapArrayHeight; y++){
            if(mapArrayOwnerTemp[x][y] != mapArrayOwner[x][y]){
                if(findBuilding(x, y) > -1){
                    bitStreamOUT.Reset();
                    bitStreamOUT.Write((RakNet::MessageID)ID_SET_BUILDING_OWNER);
                    bitStreamOUT.Write(buildingList[findBuilding(x, y)]->getBuildingOwner());
                    bitStreamOUT.Write(x);
                    bitStreamOUT.Write(y);
                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
                }
            }
        }
    }
}
