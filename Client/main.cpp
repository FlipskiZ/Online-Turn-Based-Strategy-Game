#include "Engine.h"
#include "MenuState.h"
#include "GameState.h"
#include "PlayState.h"

//Initalization +
struct TILE_TYPE{
	bool isPassable;
};

struct BUILDING_PROPERTIES{
	int metalCost;
	int foodCost;
	int oilCost;
	int silverCost;
};

bool isPassable(float x, float y, float width, float height, float deltaX, float deltaY);
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

ALLEGRO_DISPLAY *display;

ALLEGRO_FONT *defaultFont;
ALLEGRO_FONT *smallFont;
ALLEGRO_FONT *bigFont;

ALLEGRO_BITMAP *cursorImage;
ALLEGRO_BITMAP *explosionImage;

ALLEGRO_BITMAP *groundImage1;
ALLEGRO_BITMAP *groundImage2;
ALLEGRO_BITMAP *foodResourceImage;
ALLEGRO_BITMAP *oilResourceImage;
ALLEGRO_BITMAP *metalResourceImage;
ALLEGRO_BITMAP *silverResourceImage;
ALLEGRO_BITMAP *capitalBuildingImage;
ALLEGRO_BITMAP *connectorBuildingImage;
ALLEGRO_BITMAP *minerBuildingImage;

ALLEGRO_KEYBOARD_STATE keyState;
ALLEGRO_MOUSE_STATE mouseState;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;

Button *buttonList[MAX_BUTTONS];
InputField *inputFieldList[MAX_INPUT_FIELDS];
ParticleEntity *particleList[MAX_PARTICLES];
Player *playerList[MAX_PLAYERS];
std::vector<Building*> buildingList;

Engine engine;

int mapArray[maxMapArrayWidth][maxMapArrayHeight];
int mapArrayRotation[maxMapArrayWidth][maxMapArrayHeight];
int buildingIndex[maxMapArrayWidth][maxMapArrayHeight];
int mineralArray[maxMapArrayWidth][maxMapArrayHeight][maxMineralDepth][2];  ///mineralArray[x][y][Depth][0 - Type --- 1 - Quantity]

const char* versionNumber;
float fpsTimeNew, fpsCounter, fpsTimeOld;

float cameraPosX, cameraPosY, cameraOffsetX, cameraOffsetY, mapDisplayOffsetX, mapDisplayOffsetY, cameraMoveSpeed;
bool drawScreen, timerEvent, done, mouseButtonLeft, mouseButtonLeftClick, mouseButtonRight, mouseButtonRightClick, inGame, allegroWrite;
float mouseX, mouseY, volumeLevel;
int lastKeyPress, mouseWheel, mouseWheelChange;
int selectedBuildingId;

int mapArrayWidth, mapArrayHeight;

std::string allegroString;

RakNet::RakPeerInterface *rakPeer;
RakNet::Packet *rakPacket;
RakNet::SocketDescriptor rakSocketDescriptor;
int rakPort, rakClientId;
bool lanMode;
RakNet::SystemAddress serverAddress;
std::string rakIpAdress, rakClientName, serverIdSearch;

std::string chatLog[chatLogSize];

TILE_TYPE tileIndex[] = {
	{true}, // (0) TILE_GROUND1
	{true}, // (1) TILE_GROUND2
	{false}, // (2) TILE_WALL
    {false}, // (3) TILE_BROKENWALL
	{true}, // (6) TILE_FLOOR
	{true}, // (7) TILE_BROKENFLOOR
	{true}, // (8) TILE_BLOODGROUND1
	{true}, // (9) TILE_BLOODGROUND2
	{false}, // (10) TILE_VINEWALL
};

BUILDING_PROPERTIES buildingProperties[] = {
	{0, 0, 0, 0}, // (0) BUILDING_CAPITAL
	{10, 5, 5, 5}, // (1) BUILDING_CONNECTOR
	{10, 10, 20, 10}, // (2) BUILDING_MINER
	{15, 20, 30, 15}, // (3) BUILDING_WEAPON
};
//Initalization -

int main(){
    srand(time(0));

	Engine engine;

	//Initialize the Engine
	engine.init("Allegro 5 & RakNet 2D Game Engine 7-1-2015 - Client", screenWidth, screenHeight, false);

	//Load the Menu
	engine.changeState(MenuState::instance());

    //Timestep Variables
    double t = 0.0;
    double dt = 1/60.0;

    double currentTime = 0.0;
    double newTime = 0.0;
    double frameTime = 0.0;

    double accumulator = 0.0;

    int mouseWheelChangeHelper = 0;

	//Main Loop
    while(engine.running()){
        ALLEGRO_EVENT events;
        al_wait_for_event(event_queue, &events);

        timerEvent = false;

        if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            engine.quit();
        }

        if(events.type == ALLEGRO_EVENT_KEY_CHAR && allegroWrite){
            int unichar = events.keyboard.unichar;
            if(events.keyboard.keycode != ALLEGRO_KEY_BACKSPACE && events.keyboard.keycode != ALLEGRO_KEY_ENTER && events.keyboard.keycode < 90 && (events.keyboard.keycode > 85 || events.keyboard.keycode < 76) && (events.keyboard.keycode > 59 || events.keyboard.keycode < 47) && events.keyboard.keycode != ALLEGRO_KEY_BACKSLASH && events.keyboard.keycode != ALLEGRO_KEY_BACKSLASH2){
                allegroString.push_back(unichar);
            }else if(events.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && allegroString.size() > 0){
                allegroString.erase(allegroString.size()-1, 1);
            }
        }

        if(events.type == ALLEGRO_EVENT_TIMER){
            timerEvent = true;
        }
        //Main Timer Event +
        if(timerEvent){
            al_get_mouse_state(&mouseState);

            al_get_keyboard_state(&keyState);

            //Update Mouse Variables +
            mouseX = al_get_mouse_state_axis(&mouseState,0);
            mouseY = al_get_mouse_state_axis(&mouseState,1);
            mouseWheelChangeHelper = mouseWheel;
            mouseWheel = al_get_mouse_state_axis(&mouseState, 2);
            mouseWheelChange = mouseWheel-mouseWheelChangeHelper;

            if(al_mouse_button_down(&mouseState, 1)){
                mouseButtonLeft = true;
                mouseButtonLeftClick = false;
            }else if(mouseButtonLeft){
                mouseButtonLeftClick = true;
                mouseButtonLeft = false;
            }else{
                mouseButtonLeftClick = false;
            }

            if(al_mouse_button_down(&mouseState, 2)){
                mouseButtonRight = true;
                mouseButtonRightClick = false;
            }else if(mouseButtonRight){
                mouseButtonRightClick = true;
                mouseButtonRight = false;
            }else{
                mouseButtonRightClick = false;
            }
            //Update Mouse Variables -

            //Rest +
            if(!al_key_down(&keyState, lastKeyPress)){
                lastKeyPress = 0;
            }
            //Rest -

            if(events.timer.source == timer){
                newTime = al_get_time();
                frameTime = newTime - currentTime;
                if(frameTime > 0.25)
                    frameTime = 0.25;	  // note: max frame time to avoid spiral of death
                currentTime = newTime;

                accumulator += frameTime;

                while(accumulator >= dt){
                    engine.update(); //Call the gameState specfic update
                    t += dt;
                    accumulator -= dt;
                }

                engine.draw();

                al_draw_bitmap(cursorImage, mouseX-4, mouseY-4, 0);

                al_flip_display();
                al_clear_to_color(al_map_rgb(0, 0, 0));
            }
        }
        //Main Timer Event -
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

void addButtonToList(Button *newButton){
    for(int i = 0; i < MAX_BUTTONS; i++){
        if(buttonList[i] == NULL){
            newButton->setEntityId(i);
            buttonList[i] = newButton;
            return;
        }
    }
}

void addInputFieldToList(InputField *newInputField){
    for(int i = 0; i < MAX_INPUT_FIELDS; i++){
        if(inputFieldList[i] == NULL){
            newInputField->setEntityId(i);
            inputFieldList[i] = newInputField;
            return;
        }
    }
}

void addParticleToList(ParticleEntity *newParticle){
    for(int i = 0; i < MAX_PARTICLES; i++){
        if(particleList[i] == NULL){
            newParticle->setEntityId(i);
            particleList[i] = newParticle;
            return;
        }
    }
}

void addBuildingToList(Building *newBuilding){
    buildingIndex[newBuilding->getBuildingPosX()][newBuilding->getBuildingPosY()] = buildingList.size();
    newBuilding->setBuildingId(buildingList.size());
    buildingList.push_back(newBuilding);
}

void loadMapArray(){
    /*for(int y = 0; y < mapArrayHeight; y++){
        for(int x = 0; x < mapArrayWidth; x++){
            mapArray[x][y] = rand()%2;
            mapArrayRotation[x][y] = 0;
            mineralArray[x][y][0][0] = rand()%4;
            mineralArray[x][y][0][1] = 1;
            buildingIndex[x][y] = -1;
        }
    }*/
}

void saveMapArray(){
    for(int y = 0; y < mapArrayHeight; y++){
        for(int x = 0; x <= mapArrayWidth; x++){
            mapArray[x][y] = rand()%3;
            mapArrayRotation[x][y] = rand()%3;
        }
    }
}

int findBuilding(int posX, int posY){
    return buildingIndex[posX][posY];
}

void drawMap(){
    for(int x = 0; x < screenWidth/tileSize+1; x++){
        for(int y = 0; y < screenHeight/tileSize+2; y++){///The +1 and +2 are for the camera to prevent blacks spots on the sides of the screen
            int mapOffsetX = cameraPosX/tileSize, mapOffsetY = cameraPosY/tileSize;
            if(x+mapOffsetX >= 0 && x+mapOffsetX < mapArrayWidth && y+mapOffsetY >= 0 && y+mapOffsetY < mapArrayHeight){///Not to draw out of bonds in order to not waste resources and to not draw out of bounds
                drawTile(x+mapDisplayOffsetX/tileSize, y+mapDisplayOffsetY/tileSize, mapOffsetX, mapOffsetY);
            }
        }
    }
}

void drawTile(float x, float y, int mapOffsetX, int mapOffsetY){
    int arrayX = x-mapDisplayOffsetX/tileSize, arrayY = y-mapDisplayOffsetY/tileSize;
    switch(mapArray[arrayX+mapOffsetX][arrayY+mapOffsetY]){
        case 0:
            al_draw_bitmap(groundImage1, x*tileSize, y*tileSize, NULL);
            break;

        case 1:
            al_draw_bitmap(groundImage2, x*tileSize, y*tileSize, NULL);
            break;
    }

    if(mineralArray[arrayX+mapOffsetX][arrayY+mapOffsetY][0][1] > 0){///If there are minerals on the ground level
        switch(mineralArray[arrayX+mapOffsetX][arrayY+mapOffsetY][0][0]){///Draw the mineral on the ground level
            case 0:
                al_draw_bitmap(metalResourceImage, x*tileSize, y*tileSize, NULL);
                break;

            case 1:
                al_draw_bitmap(foodResourceImage, x*tileSize, y*tileSize, NULL);
                break;

            case 2:
                al_draw_bitmap(oilResourceImage, x*tileSize, y*tileSize, NULL);
                break;

            case 3:
                al_draw_bitmap(silverResourceImage, x*tileSize, y*tileSize, NULL);
                break;
        }
    }
}

void updateCamera(){
    if(cameraPosX < -leftGuiWidth){
        cameraPosX = -leftGuiWidth;
    }
    if(cameraPosX + mapDisplayWidth >= mapArrayWidth*tileSize){
        cameraPosX = mapArrayWidth*tileSize - mapDisplayWidth;
    }
    if(cameraPosY < -topGuiHeight){
        cameraPosY = -topGuiHeight;
    }
    if(cameraPosY + botGuiHeight > mapArrayHeight*tileSize){
        cameraPosY = mapArrayHeight*tileSize - botGuiHeight;
    }

    mapDisplayOffsetX = -((int)cameraPosX%tileSize);
    mapDisplayOffsetY = -((int)cameraPosY%tileSize);

    cameraOffsetX = -cameraPosX;
    cameraOffsetY = -cameraPosY;
}

bool visibleInCamera(float posX, float posY, float width, float height){
    if(posX+width+cameraOffsetX < 0 && posX+cameraOffsetX >= screenWidth && posY+height+cameraOffsetY < 0 && posY+cameraOffsetX >= screenHeight){
        return false;
    }
    return true;
}

int getBuildingProperty(int buildingType, int buildingPropertyId){
    if(buildingPropertyId == BUILDING_METAL_COST){
        return buildingProperties[buildingType].metalCost;
    }else if(buildingPropertyId == BUILDING_FOOD_COST){
        return buildingProperties[buildingType].foodCost;
    }else if(buildingPropertyId == BUILDING_OIL_COST){
        return buildingProperties[buildingType].oilCost;
    }else if(buildingPropertyId == BUILDING_SILVER_COST){
        return buildingProperties[buildingType].silverCost;
    }
}
