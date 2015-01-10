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
void addPlayerToList(Player *newPlayer);
void loadMapArray();
void saveMapArray();

Player *playerList[MAX_PLAYERS];

RakNet::RakPeerInterface *rakPeer;
RakNet::Packet *rakPacket;
RakNet::SocketDescriptor rakSocketDescriptor;

Engine engine;

int mapArray[maxMapArrayWidth][maxMapArrayHeight];
int mapArrayRotation[maxMapArrayWidth][maxMapArrayHeight];

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

    mapArrayWidth = 40;
    mapArrayHeight = 20;

    std::cout << "Which port to use? The default is 2020." << std::endl;
    std::cin >> port;

    std::cout << std::endl << "What should be the max amount of players on the server? Max: 64" << std::endl;
    std::cin >> maxPlayers;

    if(maxPlayers > 64)
        maxPlayers = 64;

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
    }
}

void addPlayerToList(Player *newPlayer){
    for(int i = 0; i < maxPlayers; i++){
        if(playerList[i] == NULL){
            newPlayer->setPlayerId(i);
            playerList[i] = newPlayer;
            return;
        }
    }
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
