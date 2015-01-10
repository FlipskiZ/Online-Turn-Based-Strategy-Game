#include "Engine.h"
#include "MenuState.h"
#include "GameState.h"
#include "PlayState.h"

PlayState PlayState::m_PlayState;

void PlayState::init(){

    //Create Player
    playerList[0] = NULL;

    //Pointer Lists +
    for(int i = 0; i < MAX_BUTTONS; i++){
        buttonList[i] = NULL;
    }
    for(int i = 0; i < MAX_PARTICLES; i++){
        particleList[i] = NULL;
    }
    //Pointer Lists -
    if(lanMode){
        RakNet::SocketDescriptor socketDescriptorTemp(rakPort+1, 0);
        rakSocketDescriptor = socketDescriptorTemp;
    }
    printf("IP-Adress: %s\nPort: %d\n", rakIpAdress.c_str(), rakPort);

    rakPeer->Startup(1, &rakSocketDescriptor, 1);
    rakPeer->Connect(rakIpAdress.c_str(), rakPort, 0, 0);

    loadMapArray();

    cameraMoveSpeed = 8;
}

void PlayState::cleanup(){
}

void PlayState::pause(){
    inGame = true;
}

void PlayState::resume(){
}

void PlayState::update(Engine* engine){

    al_get_mouse_state(&mouseState);

    al_get_keyboard_state(&keyState);

    for(rakPacket = rakPeer->Receive(); rakPacket; rakPeer->DeallocatePacket(rakPacket), rakPacket = rakPeer->Receive()){
        switch(rakPacket->data[0]){
                case ID_UNCONNECTED_PONG:{
                        RakNet::BitStream bsIn(rakPacket->data, rakPacket->length, false);
                        bsIn.IgnoreBytes(1);
                        bsIn.Read(time);
                        printf("Got pong");
                }

                case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                    printf("Another client has disconnected.\n");
                    break;

                case ID_REMOTE_CONNECTION_LOST:
                    printf("Another client has lost the connection.\n");
                    break;

                case ID_REMOTE_NEW_INCOMING_CONNECTION:
                    printf("Another client has connected.\n");
                    break;

                case ID_CONNECTION_REQUEST_ACCEPTED:{
                        printf("Our connection request has been accepted.\n");
                        serverAddress = rakPacket->systemAddress;

                        RakNet::BitStream bitStreamOUT;
                        bitStreamOUT.Write((RakNet::MessageID)ID_CONNECTED_MESSAGE);
                        bitStreamOUT.Write(rakClientName.c_str());
                        rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
                    }
                    break;

                case ID_NO_FREE_INCOMING_CONNECTIONS:
                    printf("The server is full.\n");
                    break;

                case ID_DISCONNECTION_NOTIFICATION:
                    printf("We have been disconnected.\n");
                    engine->changeState(MenuState::instance());
                    break;

                case ID_CONNECTION_LOST:
                    printf("Connection lost.\n");
                    break;

            case ID_CONNECTED_MESSAGE:{
                    RakNet::RakString rakString;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(rakString);
                    printf("%s has connected!\n", rakString.C_String());
                }
                break;

            case ID_PLAYER_INITIALIZATION:{
                    int playerTile;

                    RakNet::RakString rakString;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(playerTile);

                    playerList[0] = new Player();
                    playerList[0]->setPlayerTile(playerTile);
            }

            case ID_CHAT_MESSAGE:{
                    RakNet::RakString name;
                    RakNet::RakString message;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(name);
                    bitStreamIN.Read(message);
                    printf("%s: %s\n", name.C_String(), message.C_String());
                }
                break;

            case ID_TRANSFER_MAP_TILE:{
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    int x = 0, y = 0;

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(x);
                    bitStreamIN.Read(y);
                    bitStreamIN.Read(mapArray[x][y]);
                }
                break;

            case ID_MAP_DIMENSIONS:{
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(mapArrayWidth);
                    bitStreamIN.Read(mapArrayHeight);
            }

            default:
                printf("Message with identifier %i has arrived.\n", rakPacket->data[0]);
                break;
        }
    }

    //Player Input +
    if(al_key_down(&keyState, ALLEGRO_KEY_W)){
        cameraPosY -= cameraMoveSpeed;
    }else if(al_key_down(&keyState, ALLEGRO_KEY_S)){
        cameraPosY += cameraMoveSpeed;
    }

    if(al_key_down(&keyState, ALLEGRO_KEY_A)){
        cameraPosX -= cameraMoveSpeed;
    }else if(al_key_down(&keyState, ALLEGRO_KEY_D)){
        cameraPosX += cameraMoveSpeed;
    }

    if(mouseButtonLeftClick){
        int tX = floor(mouseX/tileSize), tY = floor(mouseY/tileSize);
        if(insideMap(tX, tY, 0, 0)){
            RakNet::BitStream bitStreamOUT;
            bitStreamOUT.Write((RakNet::MessageID)ID_PAINT_TILE);
            bitStreamOUT.Write(tX);
            bitStreamOUT.Write(tY);
            bitStreamOUT.Write(playerList[0]->getStats(1));
            rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
        }
    }

    /*if(vertical != 0 && horizontal != 0){
        playerList[0]->deltaX /= sqrt(2);
        playerList[0]->deltaY /= sqrt(2);
    }*/

    if(al_key_down(&keyState, ALLEGRO_KEY_F5)){
        if(lastKeyPress != ALLEGRO_KEY_F5){
            //loadMapArray();
            lastKeyPress = ALLEGRO_KEY_F5;
        }
    }else if(al_key_down(&keyState, ALLEGRO_KEY_ESCAPE)){
        if(lastKeyPress != ALLEGRO_KEY_ESCAPE){
            engine->pushState(MenuState::instance());
            lastKeyPress = ALLEGRO_KEY_ESCAPE;
        }
    }
    //Player Input -

    //Update Entities +
    for(int i = 0; i < MAX_PARTICLES; i++){
        if(particleList[i] != NULL){
            particleList[i]->update();
        }
    }
    for(int i = 0; i < MAX_PLAYERS; i++){
        if(playerList[i] != NULL){
            playerList[i]->update();
        }
    }
    /*for(int i = 0; i < MAX_BUTTONS; i++){
        if(buttonList[i] != NULL && buttonList[i]->checkActive()){
            buttonList[i]->update();
            if(buttonList[i]->clicked){
                switch(buttonList[i]->typeId){
                    case 3:
                        playerList[0]->buyWeapon(0,0);
                        break;
                }
            }
        }
    }*/
    //Update Entities -

    //Rest +
    updateCamera();
    //Rest -

}

void PlayState::draw(Engine* engine){
    //Draw Map+
    drawMap();
    //Draw Map-

    //Draw Entities +
    for(int i = 0; i < MAX_PLAYERS; i++){
        if(playerList[i] != NULL){
            playerList[i]->draw();
        }
    }
    for(int i = 0; i < MAX_PARTICLES; i++){
        if(particleList[i] != NULL){
            if(particleList[i]->typeId >= 2)
                particleList[i]->draw();
        }
    }
    //Draw Entities -

    //Draw GUI +
    al_draw_filled_rectangle(0, mapDisplayHeight, screenWidth, screenHeight, al_map_rgb(0,0,0));

    al_draw_textf(smallFont, al_map_rgb(150, 150, 150), screenWidth-60, screenHeight-23, NULL, "%s", versionNumber);
    fpsTimeNew = al_get_time();
    fpsCounter = 1/(fpsTimeNew - fpsTimeOld);
    fpsTimeOld = fpsTimeNew;
    al_draw_textf(defaultFont, (fpsCounter > 55) ? al_map_rgb(50, 150, 50) : (fpsCounter <= 55 && fpsCounter > 30) ? al_map_rgb(150, 150, 50) : al_map_rgb(150, 50, 50), screenWidth-95, mapDisplayHeight, NULL, "FPS: %d", (int)round(fpsCounter));
}
