#include "Engine.h"
#include "MenuState.h"
#include "GameState.h"
#include "PlayState.h"

//Initalization +
struct TILE_TYPE{
	bool isPassable;
};

bool isPassable(float x, float y, float width, float height, float deltaX, float deltaY);
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

ALLEGRO_DISPLAY *display;

ALLEGRO_FONT *defaultFont;
ALLEGRO_FONT *smallFont;
ALLEGRO_FONT *bigFont;

ALLEGRO_BITMAP *cursorImage;
ALLEGRO_BITMAP *explosionImage;

ALLEGRO_BITMAP *groundImage1;
ALLEGRO_BITMAP *groundImage2;
ALLEGRO_BITMAP *wallImage;
ALLEGRO_BITMAP *brokenWallImage;
ALLEGRO_BITMAP *floorImage;
ALLEGRO_BITMAP *brokenFloorImage;
ALLEGRO_BITMAP *bloodGroundImage1;
ALLEGRO_BITMAP *bloodGroundImage2;
ALLEGRO_BITMAP *vineWallImage;
ALLEGRO_BITMAP *foodResourceImage;
ALLEGRO_BITMAP *oilResourceImage;
ALLEGRO_BITMAP *metalResourceImage;
ALLEGRO_BITMAP *silverResourceImage;

ALLEGRO_KEYBOARD_STATE keyState;
ALLEGRO_MOUSE_STATE mouseState;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;

Button *buttonList[MAX_BUTTONS];
InputField *inputFieldList[MAX_INPUT_FIELDS];
ParticleEntity *particleList[MAX_PARTICLES];
Player *playerList[MAX_PLAYERS];

Engine engine;

int mapArray[maxMapArrayWidth][maxMapArrayHeight];
int mapArrayRotation[maxMapArrayWidth][maxMapArrayHeight];

const char* versionNumber;
float fpsTimeNew, fpsCounter, fpsTimeOld;

float cameraPosX, cameraPosY, cameraOffsetX, cameraOffsetY, mapDisplayOffsetX, mapDisplayOffsetY, cameraMoveSpeed;
bool drawScreen, timerEvent, done, mouseButtonLeft, mouseButtonLeftClick, mouseButtonRight, mouseButtonRightClick, inGame, allegroWrite;
float mouseX, mouseY, volumeLevel;
int lastKeyPress, mouseWheel = 0, mouseWheelChange;

int mapArrayWidth, mapArrayHeight;

std::string allegroString;

RakNet::RakPeerInterface *rakPeer;
RakNet::Packet *rakPacket;
RakNet::SocketDescriptor rakSocketDescriptor;
int rakPort;
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
            if(events.keyboard.keycode != ALLEGRO_KEY_BACKSPACE && events.keyboard.keycode != ALLEGRO_KEY_ENTER && events.keyboard.keycode < 90){
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

void loadMapArray(){
    std::ifstream mapArrayFile;
    mapArrayFile.open("config/MapArray.txt");

    for(int y = 0; y < mapArrayHeight; y++){
        for(int x = 0; x < mapArrayWidth; x++){
            mapArrayFile >> mapArray[x][y];

            if(mapArray[x][y] != 2 && mapArray[x][y] != 6){
                if(mapArray[x][y] == 0){
                    mapArray[x][y] = ((rand()%200 == 0) ? 8 : 0) + rand()%2;
                    mapArrayRotation[x][y] = rand()%4;
                }else if(mapArray[x][y] == 4){
                    mapArray[x][y] = 4+rand()%2;
                    mapArrayRotation[x][y] = rand()%2*2;

                }else if(mapArray[x][y] == 3 || mapArray[x][y] == 7){
                    if(mapArray[x][y] == 3 && rand()%5 == 0)
                        mapArray[x][y] = 10;
                    mapArrayRotation[x][y] = rand()%4;
                }
            }
        }
    }

    mapArrayFile.close();
}

void saveMapArray(){
    for(int y = 0; y < mapArrayHeight; y++){
        for(int x = 0; x <= mapArrayWidth; x++){
            mapArray[x][y] = rand()%3;
            mapArrayRotation[x][y] = rand()%3;
        }
    }
}

void drawMap(){
    for(int x = 0; x < screenWidth/tileSize+1; x++){
        for(int y = 0; y < screenHeight/tileSize+1; y++){
            int mapOffsetX = cameraPosX/tileSize, mapOffsetY = cameraPosY/tileSize;
            if(y+mapOffsetY >= 0 && y+mapOffsetY < mapArrayHeight)
                drawTile(x+mapDisplayOffsetX/tileSize, y+mapDisplayOffsetY/tileSize, mapArray[x+mapOffsetX][y+mapOffsetY]);
        }
    }
}

void drawTile(float x, float y, int tileId){
    switch(tileId){
        case 0:
            al_draw_rotated_bitmap(groundImage1, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            break;

        case 1:
            al_draw_rotated_bitmap(groundImage2, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            break;

        case 2:
            al_draw_bitmap(wallImage, x*tileSize, y*tileSize, NULL);
            break;

        case 3:
            al_draw_rotated_bitmap(brokenWallImage, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            break;

        case 6:
            al_draw_bitmap(floorImage, x*tileSize, y*tileSize, NULL);
            break;

        case 7:
            al_draw_rotated_bitmap(brokenFloorImage, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            break;

        case 8:
            al_draw_rotated_bitmap(bloodGroundImage1, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            break;

        case 9:
            al_draw_rotated_bitmap(bloodGroundImage2, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            break;

        case 10:
            al_draw_rotated_bitmap(vineWallImage, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            break;

        case 11:
            al_draw_rotated_bitmap(groundImage1, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            al_draw_bitmap(foodResourceImage, x*tileSize, y*tileSize, NULL);
            break;

        case 12:
            al_draw_rotated_bitmap(groundImage1, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            al_draw_bitmap(oilResourceImage, x*tileSize, y*tileSize, NULL);
            break;

        case 13:
            al_draw_rotated_bitmap(groundImage1, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            al_draw_bitmap(metalResourceImage, x*tileSize, y*tileSize, NULL);
            break;

        case 14:
            al_draw_rotated_bitmap(groundImage1, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapArrayRotation[(int)x][(int)y]*90*toRadians, NULL);
            al_draw_bitmap(silverResourceImage, x*tileSize, y*tileSize, NULL);
            break;
    }
}

void updateCamera(){
    if(cameraPosX < 0){
        cameraPosX = 0;
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

    printf("%f - %f\n", cameraPosY, cameraPosY + botGuiHeight);

    int cX = cameraPosX, cY = cameraPosY;

    mapDisplayOffsetX = -(cX%tileSize);
    mapDisplayOffsetY = -(cY%tileSize);

    cameraOffsetX = -cameraPosX;
    cameraOffsetY = -cameraPosY;
}
