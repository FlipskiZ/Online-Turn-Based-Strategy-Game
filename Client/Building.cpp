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

void Building::draw(){
    if(visibleInCamera(this->posX*tileSize, this->posY*tileSize, tileSize, tileSize)){
        switch(this->buildingType){
            case BUILDING_CAPITAL:
                al_draw_bitmap(capitalBuildingImage, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                if(this->ownerId == PLAYER_RED)
                    al_draw_tinted_bitmap(capitalBuildingImage, PLAYER_RED_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_GREEN)
                    al_draw_tinted_bitmap(capitalBuildingImage, PLAYER_GREEN_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_BLUE)
                    al_draw_tinted_bitmap(capitalBuildingImage, PLAYER_BLUE_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_YELLOW)
                    al_draw_tinted_bitmap(capitalBuildingImage, PLAYER_YELLOW_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_MAGENTA)
                    al_draw_tinted_bitmap(capitalBuildingImage, PLAYER_MAGENTA_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_CYAN)
                    al_draw_tinted_bitmap(capitalBuildingImage, PLAYER_CYAN_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_BLACK)
                    al_draw_tinted_bitmap(capitalBuildingImage, PLAYER_BLACK_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_WHITE)
                    al_draw_tinted_bitmap(capitalBuildingImage, PLAYER_WHITE_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                break;

            case BUILDING_CONNECTOR:
                al_draw_bitmap(connectorBuildingImage, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                if(this->ownerId == PLAYER_RED)
                    al_draw_tinted_bitmap(connectorBuildingImage, PLAYER_RED_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_GREEN)
                    al_draw_tinted_bitmap(connectorBuildingImage, PLAYER_GREEN_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_BLUE)
                    al_draw_tinted_bitmap(connectorBuildingImage, PLAYER_BLUE_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_YELLOW)
                    al_draw_tinted_bitmap(connectorBuildingImage, PLAYER_YELLOW_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_MAGENTA)
                    al_draw_tinted_bitmap(connectorBuildingImage, PLAYER_MAGENTA_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_CYAN)
                    al_draw_tinted_bitmap(connectorBuildingImage, PLAYER_CYAN_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_BLACK)
                    al_draw_tinted_bitmap(connectorBuildingImage, PLAYER_BLACK_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_WHITE)
                    al_draw_tinted_bitmap(connectorBuildingImage, PLAYER_WHITE_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                break;

            case BUILDING_MINER:
                al_draw_bitmap(minerBuildingImage, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                if(this->ownerId == PLAYER_RED)
                    al_draw_tinted_bitmap(minerBuildingImage, PLAYER_RED_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_GREEN)
                    al_draw_tinted_bitmap(minerBuildingImage, PLAYER_GREEN_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_BLUE)
                    al_draw_tinted_bitmap(minerBuildingImage, PLAYER_BLUE_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_YELLOW)
                    al_draw_tinted_bitmap(minerBuildingImage, PLAYER_YELLOW_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_MAGENTA)
                    al_draw_tinted_bitmap(minerBuildingImage, PLAYER_MAGENTA_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_CYAN)
                    al_draw_tinted_bitmap(minerBuildingImage, PLAYER_CYAN_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_BLACK)
                    al_draw_tinted_bitmap(minerBuildingImage, PLAYER_BLACK_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                else if(this->ownerId == PLAYER_WHITE)
                    al_draw_tinted_bitmap(minerBuildingImage, PLAYER_WHITE_COLOR_TINT, this->posX*tileSize+cameraOffsetX, this->posY*tileSize+cameraOffsetY, NULL);
                break;

        }
    }
}
