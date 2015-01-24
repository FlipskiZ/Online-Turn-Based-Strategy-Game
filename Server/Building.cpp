#include "Engine.h"

Building::Building(){
    this->posX = 0, this->posY = 0;
    this->buildingId = 0, this->buildingType = 0, this->ownerId = 0;
    this->buildingRange = 0, this-> buildingHealth = 0, this->buildingMaxHealth = 0, this->buildingAttack = 0;
}

void Building::setBuildingPos(int posX, int posY){
    this->posX = posX;
    this->posY = posY;
}
void Building::setBuildingId(int buildingId){
    this->buildingId = buildingId;
}
void Building::setBuildingType(int buildingType){
    this->buildingType = buildingType;
}
void Building::setBuildingOwner(int ownerId){
    this->ownerId = ownerId;
}
void Building::setBuildingRange(int buildingRange){
    this->buildingRange = buildingRange;
}
void Building::setBuildingHealth(int buildingHealth){
    this->buildingHealth = buildingHealth;
    this->buildingMaxHealth = buildingHealth;
}
void Building::setBuildingAttack(int buildingAttack){
    this->buildingAttack = buildingAttack;
}
void Building::takeDamage(int damage){
    this->buildingHealth -= damage;
    if(this->buildingHealth <= 0){
        deleteBuildingFromList(this->buildingId);
        connectBuildings();
    }
}
int Building::attackBuilding(int posX, int posY){
    if(abs(this->posX-posX) + abs(this->posY-posY) <= this->buildingRange && buildingList[buildingIndex[posX][posY]]->getBuildingOwner() != this->ownerId){
        buildingList[buildingIndex[posX][posY]]->takeDamage(this->buildingAttack);
        return this->buildingAttack;
    }
    return -1;
}

int Building::getBuildingPosX(){
    return this->posX;
}
int Building::getBuildingPosY(){
    return this->posY;
}
int Building::getBuildingId(){
    return this->buildingId;
}
int Building::getBuildingType(){
    return this->buildingType;
}
int Building::getBuildingOwner(){
    return this->ownerId;
}
int Building::getBuildingRange(){
    return this->buildingRange;
}
int Building::getBuildingHealth(){
    return this->buildingHealth;
}
int Building::getBuildingMaxHealth(){
    return this->buildingMaxHealth;
}
int Building::getBuildingAttack(){
    return this->buildingAttack;
}

void Building::checkConnectedNeighbours(){
    if(findBuilding(this->posX-1, this->posY) > -1){
        if(buildingList[findBuilding(this->posX-1, this->posY)]->getBuildingOwner() == -1){///Avoid infinite loop
            buildingList[findBuilding(this->posX-1, this->posY)]->setBuildingOwner(this->ownerId);
            buildingList[findBuilding(this->posX-1, this->posY)]->checkConnectedNeighbours();
        }
    }if(findBuilding(this->posX+1, this->posY) > -1){
        if(buildingList[findBuilding(this->posX+1, this->posY)]->getBuildingOwner() == -1){
            buildingList[findBuilding(this->posX+1, this->posY)]->setBuildingOwner(this->ownerId);
            buildingList[findBuilding(this->posX+1, this->posY)]->checkConnectedNeighbours();
        }
    }if(findBuilding(this->posX, this->posY-1) > -1){
        if(buildingList[findBuilding(this->posX, this->posY-1)]->getBuildingOwner() == -1){
            buildingList[findBuilding(this->posX, this->posY-1)]->setBuildingOwner(this->ownerId);
            buildingList[findBuilding(this->posX, this->posY-1)]->checkConnectedNeighbours();
        }
    }if(findBuilding(this->posX, this->posY+1) > -1){
        if(buildingList[findBuilding(this->posX, this->posY+1)]->getBuildingOwner() == -1){
            buildingList[findBuilding(this->posX, this->posY+1)]->setBuildingOwner(this->ownerId);
            buildingList[findBuilding(this->posX, this->posY+1)]->checkConnectedNeighbours();
        }
    }
    mapArrayOwner[this->posX][this->posY] = this->ownerId;
}
