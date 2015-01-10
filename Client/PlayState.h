#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "GameState.h"

class PlayState : public GameState{
    public:
        void init();
        void cleanup();

        void pause();
        void resume();

        void update(Engine* engine);
        void draw(Engine* engine);

        static PlayState* instance() {
            return &m_PlayState;
        }

    protected:
        PlayState(){}

    private:
        static PlayState m_PlayState;
};

#endif // PLAYSTATE_H
