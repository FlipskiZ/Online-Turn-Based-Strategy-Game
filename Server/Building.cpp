#include "Engine.h"

Building::Building(){
    this->posX = 0, this->posY = 0;
    this->buildingId = 0, this->buildingType = 0, this->ownerId = 0;
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
