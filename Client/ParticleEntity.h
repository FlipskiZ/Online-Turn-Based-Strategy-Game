#ifndef PARTICLEENTITY_H
#define PARTICLEENTITY_H

#include "Entity.h"

class ParticleEntity : public Entity{
    public:
        ParticleEntity();
        void draw();
        void update();

        void setDrag(float drag);
        void setDuration(int duration);
        void setMovementSpeed(float movementSpeed);
        void setValue(int value);
    protected:
    private:
        int duration;
        int durationHelper;

        int value1;

        float movementSpeed;
        float deltaX, deltaY;

        float drag;
};

#endif // PARTICLEENTITY_H
