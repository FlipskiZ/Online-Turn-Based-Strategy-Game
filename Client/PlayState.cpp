#include "Engine.h"
#include "MenuState.h"
#include "GameState.h"
#include "PlayState.h"

PlayState PlayState::m_PlayState;

void PlayState::init(){
    //Create buttons & chat
    int x = 0, y = 0, width = 0, height = 0;
    int inputFieldId = 3;

    width = 512, height = 24;

    x = 0, y = screenHeight-height;

    InputField *newField = new InputField();
    newField->setPos(x, y);
    newField->setDimensions(width, height);
    newField->setTypeId(inputFieldId);
    newField->setSelected(false);

    addInputFieldToList(newField);

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
    for(int i = 0; i < chatLogSize; i++){
        chatLog[i].clear();
    }

    if(lanMode){
        RakNet::SocketDescriptor socketDescriptorTemp(rakPort+1, 0);
        rakSocketDescriptor = socketDescriptorTemp;
    }
    printf("IP-Adress: %s\nPort: %d\n", rakIpAdress.c_str(), rakPort);

    rakPeer->Startup(1, &rakSocketDescriptor, 1);
    rakPeer->Connect(rakIpAdress.c_str(), rakPort, 0, 0);

    loadMapArray();

    cameraMoveSpeed = 8;
    chatLogPos = 0;
}

void PlayState::cleanup(){
    for(int i = 0; i < MAX_BUTTONS; i++)
        buttonList[i] = NULL;
    for(int i = 0; i < MAX_INPUT_FIELDS; i++)
        inputFieldList[i] = NULL;
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
                }break;

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
                }break;

            case ID_PLAYER_INITIALIZATION:{
                    int playerId;
                    bool thisId;
                    RakNet::RakString playerName;

                    RakNet::RakString rakString;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(playerId);
                    bitStreamIN.Read(thisId);
                    bitStreamIN.Read(playerName);
                    if(thisId){
                        rakClientId = playerId;
                    }
                    playerList[playerId] = new Player();
                    playerList[playerId]->setPlayerId(playerId);
                    playerList[playerId]->setPlayerName(playerName.C_String());
                }break;

            case ID_CHAT_MESSAGE:{
                    RakNet::RakString name;
                    RakNet::RakString message;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(name);
                    bitStreamIN.Read(message);

                    for(int i = 1; i < chatLogSize; i++){
                        chatLog[i-1] = chatLog[i];
                    }
                    chatLog[chatLogSize-1].assign(name);
                    chatLog[chatLogSize-1].append(": ");
                    chatLog[chatLogSize-1].append(message);
                }break;

            case ID_TRANSFER_MAP_TILE:{
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    int x = 0, y = 0;

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(x);
                    bitStreamIN.Read(y);
                    bitStreamIN.Read(mapArray[x][y]);
                }break;

            case ID_MAP_DIMENSIONS:{
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(mapArrayWidth);
                    bitStreamIN.Read(mapArrayHeight);
            }break;

            case ID_END_TURN:{
                    int playerId;
                    bool endTurnSetting;

                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(playerId);
                    bitStreamIN.Read(endTurnSetting);

                    playerList[playerId]->setPlayerTurn(endTurnSetting);
            }break;

            case ID_SET_RESOURCE:{
                    int resourceEnum, amount;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(resourceEnum);
                    bitStreamIN.Read(amount);

                    playerList[rakClientId]->setPlayerResource(resourceEnum, amount);
            }break;

            case ID_END_TURN_SYNCHRONIZE:{
                    int metal, food, oil, silver;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(metal);
                    bitStreamIN.Read(food);
                    bitStreamIN.Read(oil);
                    bitStreamIN.Read(silver);

                    playerList[rakClientId]->setPlayerResources(metal, food, oil, silver);
            }break;

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
        int tX = floor((mouseX+cameraOffsetX)/tileSize), tY = floor((mouseY+cameraOffsetY)/tileSize);
        if(insideMap(tX, tY, 0, 0)){
            RakNet::BitStream bitStreamOUT;
            bitStreamOUT.Write((RakNet::MessageID)ID_PLACE_BUILDING);
            bitStreamOUT.Write(tX);
            bitStreamOUT.Write(tY);
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
    }else if(al_key_down(&keyState, ALLEGRO_KEY_ENTER)){
        if(lastKeyPress != ALLEGRO_KEY_ENTER){
            bool endTurn = true;
            for(int i = 0; i < MAX_INPUT_FIELDS; i++){
                if(inputFieldList[i] != NULL){
                    if(inputFieldList[i]->getTypeId() == 3 && inputFieldList[i]->selected && inputFieldList[i]->getInput().size() > 0){
                        RakNet::BitStream bitStreamOUT;
                        bitStreamOUT.Write((RakNet::MessageID)ID_CHAT_MESSAGE);
                        bitStreamOUT.Write(inputFieldList[i]->getInput().c_str());
                        rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
                        inputFieldList[i]->clearInput();
                        endTurn = false;
                    }
                }
            }
            if(endTurn){
                RakNet::BitStream bitStreamOUT;
                bitStreamOUT.Write((RakNet::MessageID)ID_END_TURN);
                bitStreamOUT.Write(true);
                rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
            }
            lastKeyPress = ALLEGRO_KEY_ENTER;
        }
    }

    if(mouseWheelChange != 0){
        if(mouseX < 600 && mouseY > botGuiHeight){
            chatLogPos += mouseWheelChange;
            if(chatLogPos > chatLogSize-floor((screenHeight-45-botGuiHeight)/15)-1){
                chatLogPos = chatLogSize-floor((screenHeight-45-botGuiHeight)/15)-1;
            }
            if(chatLogPos < 0){
                chatLogPos = 0;
            }
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
    for(int i = 0; i < MAX_INPUT_FIELDS; i++){
        if(inputFieldList[i] != NULL){
            inputFieldList[i]->update();
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
    al_draw_filled_rectangle(0, botGuiHeight, screenWidth, screenHeight, al_map_rgba(0,0,0,127));

    for(int i = chatLogPos; i < chatLogSize; i++){
        if(screenHeight-40-15*(i-chatLogPos) >= botGuiHeight-5){
            al_draw_textf(smallFont, al_map_rgb(150, 150, 150), 24, screenHeight-(24+15)-15*(i-chatLogPos), 0, "%s", chatLog[chatLogSize-i-1].c_str());
        }
    }
    int temp = (screenHeight-30.5-botGuiHeight)/chatLogSize*floor((screenHeight-25.5-botGuiHeight)/15);///The height of the indicator in the scrollbar, which is resulted from the size in between the bottom and top of the whole scrollbar divided by how big the chatlog is times how many strings of the chatlog is drawed on screen.
    float temp2 = (screenHeight-30.5-botGuiHeight)/chatLogSize;

    al_draw_rectangle(1, botGuiHeight+1, 23, screenHeight-24, al_map_rgb(215,215,215), 1);
    al_draw_filled_rectangle(1, screenHeight-30-chatLogPos*temp2-temp, 23, screenHeight-25-chatLogPos*temp2, al_map_rgb(155,155,155));///Hard coded pixel values to work properly, not entirely sure why it doesn't work perfectly.
    for(int i = 0; i < MAX_INPUT_FIELDS; i++){
        if(inputFieldList[i] != NULL){
            inputFieldList[i]->draw();
        }
    }

    al_draw_textf(smallFont, al_map_rgb(150, 150, 150), screenWidth-60, screenHeight-23, 0, "%d", chatLogPos);
    fpsTimeNew = al_get_time();
    fpsCounter = 1/(fpsTimeNew - fpsTimeOld);
    fpsTimeOld = fpsTimeNew;
    al_draw_textf(defaultFont, (fpsCounter > 55) ? al_map_rgb(50, 150, 50) : (fpsCounter <= 55 && fpsCounter > 30) ? al_map_rgb(150, 150, 50) : al_map_rgb(150, 50, 50), screenWidth-95, botGuiHeight, 0, "FPS: %d", (int)round(fpsCounter));
}
