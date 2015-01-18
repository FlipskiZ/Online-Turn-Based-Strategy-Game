#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

Player::Player(){
    this->playerId = 0;
    this->playerName.clear();
    this->endTurn = false, this->firstBuilding = false;
    this->metalResource = 0, this->foodResource = 0, this->oilResource = 0, this->silverResource = 0;
}

void Player::setPlayerId(int id){
    this->playerId = id;
}

void Player::setPlayerGuid(RakNet::RakNetGUID playerGuid){
    this->playerGuid = playerGuid;
}

void Player::setPlayerName(const char* playerName){
    this->playerName = playerName;
}

void Player::setPlayerTurn(bool endTurn){
    this->endTurn = endTurn;
}

void Player::setPlayerFirst(bool first){
    this->firstBuilding = first;
}

void Player::setPlayerResources(int metalResource, int foodResource, int oilResource, int silverResource){
    this->metalResource = metalResource;
    this->foodResource = foodResource;
    this->oilResource = oilResource;
    this->silverResource = silverResource;
}

void Player::changePlayerResource(int resourceEnum, int amount){
    if(resourceEnum == RESOURCE_METAL){
        this->metalResource += amount;
    }else if(resourceEnum == RESOURCE_FOOD){
        this->foodResource += amount;
    }else if(resourceEnum == RESOURCE_OIL){
        this->oilResource += amount;
    }else if(resourceEnum == RESOURCE_SILVER){
        this->silverResource += amount;
    }
}

int Player::getPlayerId(){
    return this->playerId;
}

std::string Player::getPlayerName(){
    return this->playerName;
}

bool Player::getPlayerTurn(){
    return this->endTurn;
}

bool Player::getPlayerFirst(){
    return this->firstBuilding;
}

int Player::getPlayerResource(int resourceEnum){
    if(resourceEnum == RESOURCE_METAL){
        return this->metalResource;
    }else if(resourceEnum == RESOURCE_FOOD){
        return this->foodResource;
    }else if(resourceEnum == RESOURCE_OIL){
        return this->oilResource;
    }else if(resourceEnum == RESOURCE_SILVER){
        return this->silverResource;
    }
}

void Player::endPlayerTurn(){
    int posX, posY;
    bool minedMinerals = false;

    this->endTurn = false;

    for(int i = 0; i < buildingList.size(); i++){
        if(buildingList[i]->getBuildingOwner() == this->playerId){
            if(buildingList[i]->getBuildingType() == BUILDING_MINER){
                posX = buildingList[i]->getBuildingPosX(), posY = buildingList[i]->getBuildingPosY();
                if(mineralArray[posX][posY][0][0] == RESOURCE_METAL && mineralArray[posX][posY][0][1] > 0){
                    mineralArray[posX][posY][0][1] -= 10;
                    this->changePlayerResource(RESOURCE_METAL, 10);
                    minedMinerals = true;

                }else if(mineralArray[posX][posY][0][0] == RESOURCE_FOOD && mineralArray[posX][posY][0][1] > 0){
                    mineralArray[posX][posY][0][1] -= 10;
                    this->changePlayerResource(RESOURCE_FOOD, 10);
                    minedMinerals = true;

                }else if(mineralArray[posX][posY][0][0] == RESOURCE_OIL && mineralArray[posX][posY][0][1] > 0){
                    mineralArray[posX][posY][0][1] -= 10;
                    this->changePlayerResource(RESOURCE_OIL, 10);
                    minedMinerals = true;

                }else if(mineralArray[posX][posY][0][0] == RESOURCE_SILVER && mineralArray[posX][posY][0][1] > 0){
                    mineralArray[posX][posY][0][1] -= 10;
                    this->changePlayerResource(RESOURCE_SILVER, 10);
                    minedMinerals = true;
                }
            }
        }
    }

    RakNet::BitStream bitStreamOUT;
    if(minedMinerals){
        bitStreamOUT.Write((RakNet::MessageID)ID_TRANSFER_MINERAL_TILE);
        bitStreamOUT.Write(posX);
        bitStreamOUT.Write(posY);
        bitStreamOUT.Write(mineralArray[posX][posY][0][0]);
        bitStreamOUT.Write(mineralArray[posX][posY][0][1]);
        rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakPeer->GetSystemAddressFromGuid(this->playerGuid), true);

        bitStreamOUT.Reset();
        bitStreamOUT.Write((RakNet::MessageID)ID_SET_RESOURCE);
        bitStreamOUT.Write(1);
        bitStreamOUT.Write(mineralArray[posX][posY][0][0]);
        bitStreamOUT.Write(this->getPlayerResource(mineralArray[posX][posY][0][0]));
        rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
    }
    bitStreamOUT.Reset();
    bitStreamOUT.Write((RakNet::MessageID)ID_END_TURN);
    bitStreamOUT.Write(this->playerId);
    bitStreamOUT.Write(this->endTurn);
    rakPeer->Send(&bitStreamOUT, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
}
