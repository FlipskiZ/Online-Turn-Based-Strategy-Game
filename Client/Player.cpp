#include "Engine.h"
#include "MenuState.h"
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

void Player::setPlayerResource(int resourceEnum, int amount){
    if(resourceEnum == RESOURCE_METAL){
        this->metalResource = amount;
    }else if(resourceEnum == RESOURCE_FOOD){
        this->foodResource = amount;
    }else if(resourceEnum == RESOURCE_OIL){
        this->oilResource = amount;
    }else if(resourceEnum == RESOURCE_SILVER){
        this->silverResource = amount;
    }
}

int Player::getPlayerId(){
    return this->playerId;
}

std::string Player::getPlayerName(){
    return this->playerName;
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
    return 0;
}

void Player::update(){

}

void Player::draw(){

}
