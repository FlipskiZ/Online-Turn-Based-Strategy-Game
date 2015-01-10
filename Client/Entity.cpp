#include "Engine.h"
#include "Entity.h"

Entity::Entity(){
    this->posX = 0;
    this->posY = 0;
    this->centerX = 0;
    this->centerY = 0;
    this->width = 0;
    this->height = 0;
    this->angle = 0;
    this->entityId = 0;
    this->typeId = 0;
    this->sheetColums = 1;
    this->sheetRows = 1;
    this->frameWidth = 0;
    this->frameHeight = 0;
    this->animationValue = 0;
    this->animationValueHelper = 0;
    this->animationCount = 0;
    this->animationCountHelper = 0;
    this->sheetImage = NULL;
    this->frameImage = NULL;
    this->sheetImage = 0;
    this->frameImage = 0;
    this->untilFinished = false;
    this->nextFrame = false;
    this->rememberAnimationValue = 0;
    this->multiplier = 0;
    this->random = 0;
}

void Entity::setPos(float x, float y){
    this->posX = x;
    this->posY = y;
    this->updateCenter();
}
void Entity::updateCenter(){
    this->centerX = this->posX + this->width/2, this->centerY = this->posY + this->height/2;
}
void Entity::setDimensions(float width, float height){
    this->width = width;
    this->height = height;
}
void Entity::setEntityId(int entityId){
    this->entityId = entityId;
}
void Entity::setTypeId(int typeId){
    this->typeId = typeId;
}
void Entity::setAngle(float angle){
    this->angle = angle;
}
void Entity::setSheetDimensions(int sheetColums, int sheetRows, int frameWidth, int frameHeight){
    this->sheetColums = sheetColums;
    this->sheetRows = sheetRows;
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
}
void Entity::setBitmap(ALLEGRO_BITMAP *sheetImage){
    this->sheetImage = sheetImage;
    this->frameImage = al_create_sub_bitmap(sheetImage, this->frameX, this->frameY, this->frameWidth, this->frameHeight);
}
void Entity::setAnimationSpeed(int animationSpeed){
    this->animationCount = animationSpeed;
}

void Entity::setAnimationValue(int animationValue, bool untilFinished, float multiplier){
    if(untilFinished){
        this->rememberAnimationValue = animationValue;
        this->untilFinished = true;
    }
    this->animationValue = animationValue;
    this->multiplier = multiplier;
}

int Entity::getEntityId(){
    return this->entityId;
}

int Entity::getTypeId(){
    return this->typeId;
}

void Entity::updateAnimation(){

    if(this->untilFinished){
        this->animationValue = this->rememberAnimationValue;
    }

    if(this->animationValue != this->animationValueHelper){
        this->animationCountHelper = this->animationCount;
        this->animationValueHelper = this->animationValue;
        this->frameX = 0;
    }

    if(this->animationValue <= 0){
        this->frameX = 0;
        this->frameY = 0;
        this->animationCountHelper = animationCount;
        this->frameImage = al_create_sub_bitmap(this->sheetImage, this->frameX, this->frameY, this->frameWidth, this->frameHeight);
        return;
    }

    if(this->animationCountHelper*this->multiplier >= this->animationCount){
        this->frameY = this->animationValue-1;
        if(this->frameX+1 < this->sheetColums){
            this->frameX++;
        }else{
            this->frameX = 0;
            if(this->untilFinished){
                this->untilFinished = false;
            }
        }

        this->frameImage = al_create_sub_bitmap(this->sheetImage, this->frameX*this->frameWidth, this->frameY*this->frameHeight, this->frameWidth, this->frameHeight);
        this->animationCountHelper = 0;

        this->nextFrame = true;
    }else{
        this->nextFrame = false;
    }

    this->animationCountHelper++;
}
