#ifndef BUTTON_H
#define BUTTON_H

#include "Entity.h"

class Button : public Entity{
    public:
        Button(void);
        void draw();
        void update();

        bool selected, clicked;

    protected:

    private:

};

#endif // BUTTON_H
