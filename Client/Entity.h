#ifndef ENTITY_H
#define ENTITY_H

#include "Engine.h"

class Entity{
    public:
        Entity();
        void setPos(float x, float y);
        void updateCenter();
        void setDimensions(float width, float height);
        void setEntityId(int entityId);
        void setTypeId(int entityId);
        void setAngle(float angle);
        void setSheetDimensions(int sheetColums, int sheetRows, int frameWidth, int frameHeight);
        void setAnimationSpeed(int animationSpeed);
        void setBitmap(ALLEGRO_BITMAP *sheetImage);
        void updateAnimation();
        void setAnimationValue(int animationValue, bool untilFinished, float multiplier);

        int getEntityId();
        int getTypeId();

        virtual void draw() = 0;
        virtual void update() = 0;

        float posX, posY, centerX, centerY;
        float width, height;
        float angle;

        int entityId, typeId, random;

        int sheetColums, sheetRows, frameX, frameY, frameWidth, frameHeight, animationValue, animationValueHelper, animationCount, animationCountHelper;
        ALLEGRO_BITMAP *sheetImage, *frameImage;
        bool untilFinished, nextFrame;
        int rememberAnimationValue;
        float multiplier;

    protected:

    private:

};

#endif // ENTITY_H
