#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

PlayState PlayState::m_PlayState;

void PlayState::init(){

    /*//Create Player
    int width = 24, height = 24, movementSpeed = 2, sheetColums = 4, sheetRows = 3, animationSpeed = 60/movementSpeed/2;
    int health = 10, maxHealth = 10;
    playerList[0] = new Player();
    playerList[0]->setPos(mapDisplayWidth/2-width/2, mapDisplayHeight/2-height/2);
    playerList[0]->setDimensions(width, height);
    playerList[0]->setMovementSpeed(movementSpeed);
    playerList[0]->setSheetDimensions(sheetColums, sheetRows, width, height);
    playerList[0]->setAnimationSpeed(animationSpeed);
    playerList[0]->setBitmap(playerImage);
    playerList[0]->setHealth(health, maxHealth);*/

    //Pointer Lists +
    for(int i = 0; i < maxPlayers; i++){
        playerList[i] = NULL;
    }
    //Pointer Lists -

    redChosen= false, greenChosen = false, blueChosen = false, yellowChosen = false;

    //loadMapArray();
    for(int x = 0; x < mapArrayWidth; x++){
        for(int y = 0; y < mapArrayHeight; y++){
            mapArray[x][y] = 11+rand()%4;
        }
    }
}

void PlayState::cleanup(){

}

void PlayState::pause(){
}

void PlayState::resume(){
}

void PlayState::update(Engine* engine){

    for(rakPacket = rakPeer->Receive(); rakPacket; rakPeer->DeallocatePacket(rakPacket), rakPacket = rakPeer->Receive()){
        switch(rakPacket->data[0]){
            case ID_NEW_INCOMING_CONNECTION:
                printf("A connection is incoming.\n");

                break;

            case ID_DISCONNECTION_NOTIFICATION:
                printf("A client has disconnected.\n");
                break;

            case ID_CONNECTION_LOST:
                printf("A client lost the connection.\n");
                break;

            case ID_CONNECTED_MESSAGE:{
                    int playerId;
                    RakNet::RakString rakString;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(rakString);
                    printf("%s has connected!\n", rakString.C_String());

                    Player *newPlayer = new Player();
                    newPlayer->setPlayerName(rakString.C_String());
                    newPlayer->setPlayerGuid(rakPacket->guid);
                    playerId = addPlayerToList(newPlayer);

                    RakNet::BitStream bitStreamOUT;
                    bitStreamOUT.Write((RakNet::MessageID)ID_CONNECTED_MESSAGE);
                    bitStreamOUT.Write(rakString.C_String());
                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);

                    bitStreamOUT.Reset();
                    bitStreamOUT.Write((RakNet::MessageID)ID_MAP_DIMENSIONS);
                    bitStreamOUT.Write(mapArrayWidth);
                    bitStreamOUT.Write(mapArrayHeight);
                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, rakPacket->systemAddress, false);

                    for(int x = 0; x < mapArrayWidth; x++){
                        for(int y = 0; y < mapArrayHeight; y++){
                            bitStreamOUT.Reset();
                            bitStreamOUT.Write((RakNet::MessageID)ID_TRANSFER_MAP_TILE);
                            bitStreamOUT.Write(x);
                            bitStreamOUT.Write(y);
                            bitStreamOUT.Write(mapArray[x][y]);
                            rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, rakPacket->systemAddress, false);
                        }
                    }
                    for(int i = 0; playerList[i] != NULL; i++){
                        if(i != playerId){
                            bitStreamOUT.Reset();
                            bitStreamOUT.Write((RakNet::MessageID)ID_PLAYER_INITIALIZATION);
                            bitStreamOUT.Write(i);
                            bitStreamOUT.Write(false);
                            bitStreamOUT.Write(playerList[i]->getPlayerName());

                            rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, rakPacket->systemAddress, false);
                        }
                    }

                    bitStreamOUT.Reset();
                    bitStreamOUT.Write((RakNet::MessageID)ID_PLAYER_INITIALIZATION);
                    bitStreamOUT.Write(playerId);
                    bitStreamOUT.Write(false);
                    bitStreamOUT.Write(playerList[playerId]->getPlayerName());

                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, rakPacket->systemAddress, true);

                    bitStreamOUT.Reset();
                    bitStreamOUT.Write((RakNet::MessageID)ID_PLAYER_INITIALIZATION);
                    bitStreamOUT.Write(playerId);
                    bitStreamOUT.Write(true);
                    bitStreamOUT.Write(playerList[playerId]->getPlayerName());

                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, rakPacket->systemAddress, false);
                    {
                        int posX, posY, playerId;
                        for(int i = 0; i < buildingList.size(); i++){
                            posX = buildingList[i]->getBuildingPosX();
                            posY = buildingList[i]->getBuildingPosY();
                            playerId = buildingList[i]->getBuildingOwner();

                            bitStreamOUT.Reset();
                            bitStreamOUT.Write((RakNet::MessageID)ID_PLACE_BUILDING);
                            bitStreamOUT.Write(playerId);
                            bitStreamOUT.Write(posX);
                            bitStreamOUT.Write(posY);
                            bitStreamOUT.Write(0);///Building Type
                            rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, rakPacket->systemAddress, false);
                        }
                    }
                }break;

            case ID_CHAT_MESSAGE:{
                    int playerId = findPlayer(rakPacket->guid);
                    RakNet::RakString message;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(message);

                    RakNet::BitStream bitStreamOUT;
                    bitStreamOUT.Write((RakNet::MessageID)ID_CHAT_MESSAGE);
                    bitStreamOUT.Write(playerList[playerId]->getPlayerName().c_str());
                    printf("%s: %s\n", playerList[playerId]->getPlayerName().c_str(), message.C_String());
                    bitStreamOUT.Write(message.C_String());
                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
                }break;

            case ID_PLACE_BUILDING:{
                    int playerId = findPlayer(rakPacket->guid);
                    int posX, posY;

                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(posX);
                    bitStreamIN.Read(posY);

                    Building *newBuilding = new Building();
                    newBuilding->setBuildingPos(posX, posY);
                    newBuilding->setBuildingType(0);
                    newBuilding->setBuildingOwner(playerId);
                    addBuildingToList(newBuilding);

                    RakNet::BitStream bitStreamOUT;
                    bitStreamOUT.Write((RakNet::MessageID)ID_PLACE_BUILDING);
                    bitStreamOUT.Write(playerId);
                    bitStreamOUT.Write(posX);
                    bitStreamOUT.Write(posY);
                    bitStreamOUT.Write(0);///Building Type
                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
                }break;

            case ID_END_TURN:{
                    int playerId = findPlayer(rakPacket->guid);
                    bool endTurnSetting;

                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(endTurnSetting);

                    playerList[playerId]->setPlayerTurn(endTurnSetting);

                    RakNet::BitStream bitStreamOUT;
                    bitStreamOUT.Write((RakNet::MessageID)ID_END_TURN);
                    bitStreamOUT.Write(playerId);
                    bitStreamOUT.Write(endTurnSetting);
                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);

                    if(endTurn()){
                        RakNet::BitStream bitStreamOUT;

                        for(int i = 0; i < maxPlayers; i++){
                            if(playerList[i] != NULL){
                                bitStreamOUT.Reset();
                                bitStreamOUT.Write((RakNet::MessageID)ID_END_TURN_SYNCHRONIZE);
                                bitStreamOUT.Write(playerList[i]->getPlayerResource(RESOURCE_METAL));
                                bitStreamOUT.Write(playerList[i]->getPlayerResource(RESOURCE_FOOD));
                                bitStreamOUT.Write(playerList[i]->getPlayerResource(RESOURCE_OIL));
                                bitStreamOUT.Write(playerList[i]->getPlayerResource(RESOURCE_SILVER));
                                rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, rakPacket->systemAddress, false);

                            }
                        }
                    }
                }break;

            default:
                printf("Message with identifier %i has arrived.\n", rakPacket->data[0]);
                break;
        }
    }

    if(kbhit()){
        char commandChar;
        commandChar = getche();
        if(commandChar == '\b' && serverCommand.size() > 0){
            serverCommand.pop_back();
            printf(" \b");
        }else if(commandChar == '\r' && serverCommand.size() > 0){
            enterCommand(serverCommand);
            serverCommand.clear();
        }else{
            serverCommand.push_back(commandChar);
        }
    }
}
