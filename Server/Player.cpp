#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

Player::Player(){
    this->playerId = 0;
    this->playerName.clear();
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

int Player::getStats(int id){
    switch(id){
        case 0:
            return playerId;
            break;

        default:
            return 0;
            break;
    }
    return 0;
}

std::string Player::getPlayerName(){
    return this->playerName;
}


void Player::update(){

}
