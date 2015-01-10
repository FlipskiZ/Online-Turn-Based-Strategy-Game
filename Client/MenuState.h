#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "GameState.h"
#include "PlayState.h"

class MenuState : public GameState{
    public:
        void init();
        void cleanup();

        void pause();
        void resume();

        void update(Engine* engine);
        void draw(Engine* engine);

        static MenuState* instance() {
            return &m_MenuState;
        }

    protected:
        MenuState(){}

    private:
        static MenuState m_MenuState;

        int menuId, menuIdCopy;
};

#endif // MENUSTATE_H
