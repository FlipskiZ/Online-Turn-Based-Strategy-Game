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
    buildingList.clear();
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
                    int metalResource, foodResource, oilResource, silverResource;

                    RakNet::RakString rakString;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(playerId);
                    bitStreamIN.Read(thisId);
                    bitStreamIN.Read(playerName);
                    if(thisId){
                        bitStreamIN.Read(metalResource);
                        bitStreamIN.Read(foodResource);
                        bitStreamIN.Read(oilResource);
                        bitStreamIN.Read(silverResource);
                        rakClientId = playerId;
                    }

                    playerList[playerId] = new Player();
                    playerList[playerId]->setPlayerId(playerId);
                    playerList[playerId]->setPlayerName(playerName.C_String());
                    playerList[playerId]->setPlayerResources(metalResource, foodResource, oilResource, silverResource);
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

            case ID_PLACE_BUILDING:{
                    int playerId;
                    int posX, posY, buildingType;
                    int buildingRange, buildingHealth, buildingAttack;

                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(playerId);
                    bitStreamIN.Read(posX);
                    bitStreamIN.Read(posY);
                    bitStreamIN.Read(buildingType);
                    bitStreamIN.Read(buildingHealth);
                    bitStreamIN.Read(buildingRange);
                    bitStreamIN.Read(buildingAttack);

                    Building *newBuilding = new Building();
                    newBuilding->setBuildingPos(posX, posY);
                    newBuilding->setBuildingType(buildingType);
                    newBuilding->setBuildingOwner(playerId);
                    newBuilding->setBuildingHealth(buildingHealth);
                    newBuilding->setBuildingRange(buildingRange);
                    newBuilding->setBuildingAttack(buildingAttack);
                    addBuildingToList(newBuilding);
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
                    int resourceEnum, amount, loopAmount;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(loopAmount);
                    for(int i = 0; i < loopAmount; i++){
                        bitStreamIN.Read(resourceEnum);
                        bitStreamIN.Read(amount);
                        playerList[rakClientId]->setPlayerResource(resourceEnum, amount);
                    }
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

            case ID_TRANSFER_MINERAL_TILE:{
                    int posX, posY, mineralId, mineralQuantity;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(posX);
                    bitStreamIN.Read(posY);
                    bitStreamIN.Read(mineralId);
                    bitStreamIN.Read(mineralQuantity);

                    mineralArray[posX][posY][0][0] = mineralId;
                    mineralArray[posX][posY][0][1] = mineralQuantity;
            }break;

            case ID_SET_BUILDING_OWNER:{
                    int posX, posY, buildingOwner, buildingId;

                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);

                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(buildingOwner);
                    bitStreamIN.Read(posX);
                    bitStreamIN.Read(posY);
                    buildingId = findBuilding(posX, posY);
                    if(buildingId > -1){
                        buildingList[buildingId]->setBuildingOwner(buildingOwner);
                        printf("X: %d - Y: %d - ID: %d - OWN: %d", posX, posY, buildingId, buildingOwner);
                    }
            }break;

            case ID_ATTACK_BUILDING:{
                    int posX, posY, attackDamage;

                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(posX);
                    bitStreamIN.Read(posY);
                    bitStreamIN.Read(attackDamage);

                    buildingList[buildingIndex[posX][posY]]->takeDamage(attackDamage);
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

    if(mouseButtonLeftClick && mouseX >= leftGuiWidth && mouseX < rightGuiWidth && mouseY >= topGuiHeight && mouseY < botGuiHeight){
        int tX = floor((mouseX+cameraPosX)/tileSize), tY = floor((mouseY+cameraPosY)/tileSize);

        if(selectedBuildingX > -1 && selectedBuildingY > -1){
            if(buildingList[buildingIndex[selectedBuildingX][selectedBuildingY]]->getBuildingOwner() != rakClientId && buildingList[buildingIndex[selectedBuildingX][selectedBuildingY]]->getBuildingRange() > 0){
                RakNet::BitStream bitStreamOUT;
                bitStreamOUT.Write((RakNet::MessageID)ID_ATTACK_BUILDING);
                bitStreamOUT.Write(selectedBuildingX);
                bitStreamOUT.Write(selectedBuildingY);
                bitStreamOUT.Write(tX);
                bitStreamOUT.Write(tY);
                rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
            }
            selectedBuildingX = -1, selectedBuildingY = -1;
        }else{
            if(buildingIndex[tX][tY] > -1){
                if(buildingList[buildingIndex[tX][tY]]->getBuildingOwner() == rakClientId){
                    selectedBuildingX = tX, selectedBuildingY = tY;
                }
            }else if(buildingIndex[tX][tY] == -1){
                if(insideMap(tX, tY, 0, 0)){
                    RakNet::BitStream bitStreamOUT;
                    bitStreamOUT.Write((RakNet::MessageID)ID_PLACE_BUILDING);
                    bitStreamOUT.Write(tX);
                    bitStreamOUT.Write(tY);
                    bitStreamOUT.Write(selectedBuildingId);
                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
                }
            }
        }
    }else if(mouseButtonLeftClick){
        selectedBuildingX = -1, selectedBuildingY = -1;
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
    }else if(al_key_down(&keyState, ALLEGRO_KEY_1)){
        if(lastKeyPress != ALLEGRO_KEY_1){
            selectedBuildingId = BUILDING_CONNECTOR;
            lastKeyPress = ALLEGRO_KEY_1;
        }
    }else if(al_key_down(&keyState, ALLEGRO_KEY_2)){
        if(lastKeyPress != ALLEGRO_KEY_2){
            selectedBuildingId = BUILDING_MINER;
            lastKeyPress = ALLEGRO_KEY_2;
        }
    }else if(al_key_down(&keyState, ALLEGRO_KEY_3)){
        if(lastKeyPress != ALLEGRO_KEY_3){
            selectedBuildingId = BUILDING_SMALLWEAPON;
            lastKeyPress = ALLEGRO_KEY_3;
        }
    }else if(al_key_down(&keyState, ALLEGRO_KEY_4)){
        if(lastKeyPress != ALLEGRO_KEY_4){
            selectedBuildingId = BUILDING_BIGWEAPON;
            lastKeyPress = ALLEGRO_KEY_4;
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

    for(int i = 0; i < buildingList.size(); i++){
        buildingList[i]->draw();
    }
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
    al_draw_filled_rectangle(0, botGuiHeight, screenWidth, screenHeight, al_map_rgba(0,0,0,127));///Draw the bot GUI
    al_draw_filled_rectangle(0, 0, screenWidth, topGuiHeight, al_map_rgba(0,0,0,127));///Draw the top GUI
    al_draw_filled_rectangle(0, topGuiHeight, leftGuiWidth, botGuiHeight, al_map_rgba(0,0,0,127));///Draw the Left GUI

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

    ///Drawing the players resources
    if(playerList[rakClientId] != NULL){
        al_draw_bitmap(metalResourceImage, 0, 0, NULL); al_draw_textf(smallFont, al_map_rgb(150, 150, 150), 32, 0, 0, "%d", playerList[rakClientId]->getPlayerResource(RESOURCE_METAL));
        al_draw_bitmap(foodResourceImage, 128, 0, NULL); al_draw_textf(smallFont, al_map_rgb(150, 150, 150), 160, 0, 0, "%d", playerList[rakClientId]->getPlayerResource(RESOURCE_FOOD));
        al_draw_bitmap(oilResourceImage, 256, 0, NULL); al_draw_textf(smallFont, al_map_rgb(150, 150, 150), 288, 0, 0, "%d", playerList[rakClientId]->getPlayerResource(RESOURCE_OIL));
        al_draw_bitmap(silverResourceImage, 384, 0, NULL); al_draw_textf(smallFont, al_map_rgb(150, 150, 150), 416, 0, 0, "%d", playerList[rakClientId]->getPlayerResource(RESOURCE_SILVER));
    }

    ///Drawing the other players color, the first 3 letters of their name and if they have finished their turn on the left GUI
    for(int i = 0; i < MAX_PLAYERS; i++){
        if(playerList[i] != NULL){
            if(i == PLAYER_RED){
                al_draw_filled_rectangle(0, topGuiHeight+32*i, 32, topGuiHeight+32+32*i, PLAYER_RED_COLOR);
            }else if(i == PLAYER_GREEN){
                al_draw_filled_rectangle(0, topGuiHeight+32*i, 32, topGuiHeight+32+32*i, PLAYER_GREEN_COLOR);
            }else if(i == PLAYER_BLUE){
                al_draw_filled_rectangle(0, topGuiHeight+32*i, 32, topGuiHeight+32+32*i, PLAYER_BLUE_COLOR);
            }else if(i == PLAYER_YELLOW){
                al_draw_filled_rectangle(0, topGuiHeight+32*i, 32, topGuiHeight+32+32*i, PLAYER_YELLOW_COLOR);
            }else if(i == PLAYER_MAGENTA){
                al_draw_filled_rectangle(0, topGuiHeight+32*i, 32, topGuiHeight+32+32*i, PLAYER_MAGENTA_COLOR);
            }else if(i == PLAYER_CYAN){
                al_draw_filled_rectangle(0, topGuiHeight+32*i, 32, topGuiHeight+32+32*i, PLAYER_CYAN_COLOR);
            }else if(i == PLAYER_BLACK){
                al_draw_filled_rectangle(0, topGuiHeight+32*i, 32, topGuiHeight+32+32*i, PLAYER_BLACK_COLOR);
            }else if(i == PLAYER_WHITE){
                al_draw_filled_rectangle(0, topGuiHeight+32*i, 32, topGuiHeight+32+32*i, PLAYER_WHITE_COLOR);
            }
            al_draw_textf(smallFont, al_map_rgb(150, 150, 150), 16, (topGuiHeight+32*i)+(32-al_get_font_line_height(smallFont))/2, ALLEGRO_ALIGN_CENTER, playerList[i]->getPlayerName().substr(0, 3).c_str());///Writing the first 3 letters of a players name on their color
            al_draw_filled_rectangle(32, topGuiHeight+32*i, 64, topGuiHeight+32+32*i, (playerList[i]->getPlayerTurn()) ? al_map_rgb(50, 200, 50) : al_map_rgb(200, 50, 50));
        }
    }

    ///Drawing the selected building
    al_draw_text(smallFont, al_map_rgb(150,150,150), screenWidth-200, botGuiHeight+8, NULL, "Selected building:");
    if(selectedBuildingId == BUILDING_CONNECTOR){
        al_draw_bitmap(connectorBuildingImage, screenWidth-40, botGuiHeight, NULL);
    }else if(selectedBuildingId == BUILDING_MINER){
        al_draw_bitmap(minerBuildingImage, screenWidth-40, botGuiHeight, NULL);
    }
    al_draw_textf(smallFont, al_map_rgb(150,150,150), screenWidth-200, botGuiHeight+32, NULL, "Metal cost: %d", getBuildingProperty(selectedBuildingId, BUILDING_METAL_COST));
    al_draw_textf(smallFont, al_map_rgb(150,150,150), screenWidth-200, botGuiHeight+46, NULL, "Food cost: %d", getBuildingProperty(selectedBuildingId, BUILDING_FOOD_COST));
    al_draw_textf(smallFont, al_map_rgb(150,150,150), screenWidth-200, botGuiHeight+60, NULL, "Oil cost: %d", getBuildingProperty(selectedBuildingId, BUILDING_OIL_COST));
    al_draw_textf(smallFont, al_map_rgb(150,150,150), screenWidth-200, botGuiHeight+75, NULL, "Silver cost: %d", getBuildingProperty(selectedBuildingId, BUILDING_SILVER_COST));

    fpsTimeNew = al_get_time();
    fpsCounter = 1/(fpsTimeNew - fpsTimeOld);
    fpsTimeOld = fpsTimeNew;
    al_draw_textf(defaultFont, (fpsCounter > 55) ? al_map_rgb(50, 150, 50) : (fpsCounter <= 55 && fpsCounter > 30) ? al_map_rgb(150, 150, 50) : al_map_rgb(150, 50, 50), screenWidth-90, 0, 0, "FPS: %d", (int)round(fpsCounter));
    al_draw_textf(smallFont, al_map_rgb(150, 150, 150), screenWidth, 20, ALLEGRO_ALIGN_RIGHT, "%s", versionNumber);
}
