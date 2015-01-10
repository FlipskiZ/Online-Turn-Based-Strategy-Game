#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

void Engine::init(const char* title){

    SetConsoleTitle(title);

    m_running = true;

    versionNumber = "v1.0";

    done = false;
    port = 0, maxPlayers = 0;
    serverCommand.clear();

    loadMapArray();
}

void Engine::cleanup(){
	// cleanup the all states

	while(!states.empty()){
		states.back()->cleanup();
		states.pop_back();
	}
}

void Engine::changeState(GameState* state) {
	// cleanup the current state
	if(!states.empty()){
		states.back()->cleanup();
		states.pop_back();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->cleanup();
	states.back()->init();
}

void Engine::pushState(GameState* state){
	// pause current state
	if (!states.empty()){
		states.back()->pause();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->init();
}

void Engine::popState(){
	// cleanup the current state
	if (!states.empty()){
		states.back()->cleanup();
		states.pop_back();
	}

	// resume previous state
	if (!states.empty()){
		states.back()->resume();
	}
}

void Engine::update(){
	// let the state update the game
	states.back()->update(this);
}
