#include "Engine.h"

Building::Building(){
    this->posX = 0, this->posY = 0;
    this->buildingId = 0, this->buildingType = 0, this->ownerId = 0;
    this->capital = false;
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
void Building::setBuildingCapital(bool capital){
    this->capital = capital;
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
bool Building::getBuildingCapital(){
    return this->capital;
}

void Building::checkConnectedNeighbours(int posX, int posY){
    for(int i = 0; i < buildingList.size(); i++){
        if(buildingList[i]->getBuildingPosX() == posX-1 && buildingList[i]->getBuildingPosY() == posY && buildingList[i]->getBuildingOwner() == -1){
            buildingList[i]->setBuildingOwner(this->ownerId);
            buildingList[i]->checkConnectedNeighbours(buildingList[i]->getBuildingPosX(), buildingList[i]->getBuildingPosY());

        }else if(buildingList[i]->getBuildingPosX() == posX+1 && buildingList[i]->getBuildingPosY() == posY){
            buildingList[i]->setBuildingOwner(this->ownerId);
            buildingList[i]->checkConnectedNeighbours(buildingList[i]->getBuildingPosX(), buildingList[i]->getBuildingPosY());

        }else if(buildingList[i]->getBuildingPosX() == posX && buildingList[i]->getBuildingPosY() == posY-1){
            buildingList[i]->setBuildingOwner(this->ownerId);
            buildingList[i]->checkConnectedNeighbours(buildingList[i]->getBuildingPosX(), buildingList[i]->getBuildingPosY());

        }else if(buildingList[i]->getBuildingPosX() == posX && buildingList[i]->getBuildingPosY() == posY+1){
            buildingList[i]->setBuildingOwner(this->ownerId);
            buildingList[i]->checkConnectedNeighbours(buildingList[i]->getBuildingPosX(), buildingList[i]->getBuildingPosY());

        }
    }
}
