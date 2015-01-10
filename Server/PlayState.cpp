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

    loadMapArray();
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
                    int randomTile;
                    RakNet::RakString rakString;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(rakString);
                    printf("%s has connected!\n", rakString.C_String());

                    Player *newPlayer = new Player();
                    newPlayer->setPlayerName(rakString.C_String());
                    newPlayer->setPlayerAddress(rakPacket->systemAddress);

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
                    bitStreamOUT.Reset();
                    bitStreamOUT.Write((RakNet::MessageID)ID_PLAYER_INITIALIZATION);

                    do{
                        randomTile = 11+rand()%4;
                    }while(((randomTile == 11 && redChosen) || (randomTile == 12 && greenChosen) || (randomTile == 13 && blueChosen) || (randomTile == 14 && yellowChosen)) && !(redChosen && greenChosen && blueChosen && yellowChosen));
                    if(randomTile == 11)
                        redChosen = true;
                    else if(randomTile == 11)
                        greenChosen = true;
                    else if(randomTile == 12)
                        blueChosen = true;
                    else if(randomTile == 13)
                        yellowChosen = true;

                    bitStreamOUT.Write(randomTile);

                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, rakPacket->systemAddress, false);
                }
                break;

            case ID_CHAT_MESSAGE:{
                    RakNet::RakString name;
                    RakNet::RakString message;
                    RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                    bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                    bitStreamIN.Read(name);
                    bitStreamIN.Read(message);
                    printf("%s: %s\n", name.C_String(), message.C_String());

                    RakNet::BitStream bitStreamOUT;
                    bitStreamOUT.Write((RakNet::MessageID)ID_CHAT_MESSAGE);
                    bitStreamOUT.Write(name.C_String());
                    bitStreamOUT.Write(message.C_String());
                    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
                }
                break;

            case ID_PAINT_TILE:{
                int x, y, coloredTile;

                RakNet::BitStream bitStreamIN(rakPacket->data, rakPacket->length, false);
                bitStreamIN.IgnoreBytes(sizeof(RakNet::MessageID));
                bitStreamIN.Read(x);
                bitStreamIN.Read(y);
                bitStreamIN.Read(coloredTile);
                if(insideMap(x, y, 0, 0)){
                    if((mapArray[x][y] == 0 || mapArray[x][y] == 1 || mapArray[x][y] == 8 || mapArray[x][y] == 9 || (mapArray[x][y] > 10 && mapArray[x][y] < 15)) && coloredTile > 10 && coloredTile < 15){
                        mapArray[x][y] = coloredTile;

                        RakNet::BitStream bitStreamOUT;
                        bitStreamOUT.Write((RakNet::MessageID)ID_TRANSFER_MAP_TILE);
                        bitStreamOUT.Write(x);
                        bitStreamOUT.Write(y);
                        bitStreamOUT.Write(coloredTile);
                        rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
                    }
                }
            }
            break;

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