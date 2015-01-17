#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

Player::Player(){
    this->playerId = 0;
    this->playerName.clear();
    this->endTurn = false;
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

    this->endTurn = false;

    for(int i = 0; i < buildingList.size(); i++){
        if(buildingList[i]->getBuildingOwner() == this->playerId){
            posX = buildingList[i]->getBuildingPosX(), posY = buildingList[i]->getBuildingPosY();
            if(buildingList[i]->getBuildingType() == 0){
                if(mineralArray[posX][posY][0][1] == RESOURCE_METAL && mineralArray[posX][posY][0][2] > 0){
                    mineralArray[posX][posY][0][2] -= 10;
                    this->changePlayerResource(RESOURCE_METAL, 10);

                }else if(mineralArray[posX][posY][0][1] == RESOURCE_FOOD && mineralArray[posX][posY][0][2] > 0){
                    mineralArray[posX][posY][0][2] -= 10;
                    this->changePlayerResource(RESOURCE_FOOD, 10);

                }else if(mineralArray[posX][posY][0][1] == RESOURCE_OIL && mineralArray[posX][posY][0][2] > 0){
                    mineralArray[posX][posY][0][2] -= 10;
                    this->changePlayerResource(RESOURCE_OIL, 10);

                }else if(mineralArray[posX][posY][0][1] == RESOURCE_SILVER && mineralArray[posX][posY][0][2] > 0){
                    mineralArray[posX][posY][0][2] -= 10;
                    this->changePlayerResource(RESOURCE_SILVER, 10);
                }
            }
        }
    }
}
