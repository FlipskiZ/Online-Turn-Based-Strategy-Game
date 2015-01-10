#include "Engine.h"
#include "MenuState.h"
#include "GameState.h"
#include "PlayState.h"

Player::Player(){
    this->playerId = 0;
}

void Player::setPlayerId(int id){
    this->playerId = id;
}

void Player::setPlayerTile(int playerTile){
    this->playerTile = playerTile;
}

int Player::getStats(int id){
    switch(id){
        case 0:
            return this->playerId;
            break;

        case 1:
            return this->playerTile;
            break;

        default:
            return 0;
            break;
    }
    return 0;
}

void Player::update(){

}

void Player::draw(){

}
