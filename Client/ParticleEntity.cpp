#include "Engine.h"
#include "ParticleEntity.h"

ParticleEntity::ParticleEntity(){
    this->duration = 0;
    this->durationHelper = 0;
    this->drag = 0;
    this->movementSpeed = 0;
    this->deltaX = 0;
    this->deltaY = 0;
    this->value1 = 0;
    this->random = rand();
}
void ParticleEntity::setMovementSpeed(float movementSpeed){
    this->movementSpeed = movementSpeed;
}
void ParticleEntity::setDrag(float drag){
    this->drag = drag;
}
void ParticleEntity::setDuration(int duration){
    this->duration = duration;
}
void ParticleEntity::setValue(int value){
    this->value1 = value;
}

void ParticleEntity::update(){
    this->movementSpeed *= this->drag;

    this->deltaX = sin(this->angle) * this->movementSpeed;
    this->deltaY = -cos(this->angle) * this->movementSpeed;

    this->posX += this->deltaX;
    this->posY += this->deltaY;

    if(this->durationHelper >= this->duration){
        particleList[this->entityId] = NULL;
    }else{
        this->durationHelper++;
    }
}

void ParticleEntity::draw(){

    switch(this->typeId){
        case 0:
            al_draw_filled_rectangle(this->posX+cameraOffsetX, this->posY+cameraOffsetY, this->posX+this->width, this->posY+this->height, al_map_rgb(100+this->random%51, 25, 25));
            break;

        case 1:
            al_draw_filled_rectangle(this->posX+cameraOffsetX, this->posY+cameraOffsetY, this->posX+this->width, this->posY+this->height, al_map_rgb(200+this->random%56, 0, 0));
            break;

        case 2:
            al_draw_textf(smallFont, al_map_rgb(50, 200, 50), this->posX+cameraOffsetX, this->posY+cameraOffsetY, ALLEGRO_ALIGN_CENTER, "+%d$", this->value1);
            break;

        case 3:
            al_draw_textf(smallFont, al_map_rgb(50, 50, 200), this->posX+cameraOffsetX, this->posY+cameraOffsetY, ALLEGRO_ALIGN_CENTER, "+%dXP", this->value1);
            break;

        case 4:
            al_draw_textf(bigFont, al_map_rgb(50, 255, 50), this->posX+cameraOffsetX, this->posY+cameraOffsetY, ALLEGRO_ALIGN_CENTER, "LEVEL UP!");
            break;

        case 5:
            al_draw_textf(smallFont, al_map_rgb(200, 50, 50), this->posX+cameraOffsetX, this->posY+cameraOffsetY, ALLEGRO_ALIGN_CENTER, "-%d", this->value1);
            break;
    }
}
