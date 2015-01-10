#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Engine.h"

class GameState{
    public:
        virtual void init(){}
        virtual void cleanup(){}

        virtual void pause(){}
        virtual void resume(){}

        virtual void update(Engine* engine){}
        virtual void draw(Engine* engine){}

        void changeState(Engine* engine, GameState* gameState){
            engine->changeState(gameState);
        }

    protected:
        GameState(){}
};

#endif
