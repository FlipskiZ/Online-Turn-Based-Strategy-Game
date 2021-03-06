#include "Engine.h"
#include "MenuState.h"
#include "GameState.h"
#include "PlayState.h"

void Engine::init(const char* title, int width, int height, bool fullscreen){

	// initialize ALLEGRO
	al_init();
    if(fullscreen){
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	}else{
        al_set_new_display_flags(ALLEGRO_WINDOWED);
	}
	al_set_new_display_flags(ALLEGRO_OPENGL);
    display = al_create_display(screenWidth, screenHeight);
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_acodec_addon();
    al_install_keyboard();
    al_install_mouse();
    al_install_audio();
    al_set_window_title(display, title);

    bigFont = al_load_font("fonts/pixelFont.ttf", 48, 0);
    defaultFont = al_load_font("fonts/pixelFont.ttf", 24, 0);
    smallFont = al_load_font("fonts/pixelFont.ttf", 16, 0);

    cursorImage = al_load_bitmap("graphics/cursorImage.png");

    groundImage1 = al_load_bitmap("graphics/groundImage1.png");
    groundImage2 = al_load_bitmap("graphics/groundImage2.png");
    foodResourceImage = al_load_bitmap("graphics/foodResourceImage.png");
    oilResourceImage = al_load_bitmap("graphics/oilResourceImage.png");
    metalResourceImage = al_load_bitmap("graphics/metalResourceImage.png");
    silverResourceImage = al_load_bitmap("graphics/silverResourceImage.png");
    capitalBuildingImage = al_load_bitmap("graphics/capitalBuildingImage.png");
    connectorBuildingImage = al_load_bitmap("graphics/connectorBuildingImage.png");
    minerBuildingImage = al_load_bitmap("graphics/minerBuildingImage.png");
    smallWeaponBuildingImage = al_load_bitmap("graphics/smallWeaponBuildingImage.png");
    bigWeaponBuildingImage = al_load_bitmap("graphics/bigWeaponBuildingImage.png");

    event_queue = al_create_event_queue();
    timer = al_create_timer(1/FPS);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_hide_mouse_cursor(display);

	m_fullscreen = fullscreen;
    m_running = true;

    //Pointer Lists +
    for(int i = 0; i < MAX_BUTTONS; i++){
        buttonList[i] = NULL;
    }
    for(int i = 0; i < MAX_PLAYERS; i++){
        playerList[i] = NULL;
    }
    for(int i = 0; i < MAX_PARTICLES; i++){
        particleList[i] = NULL;
    }
    //Pointer Lists -

    //Variables +
    versionNumber = "v0.01";

    fpsTimeNew = 0, fpsCounter = 0, fpsTimeOld = 0;

    drawScreen = false, timerEvent = false, done = false, mouseButtonLeft = false, mouseButtonLeftClick = false, mouseButtonRight = false, mouseButtonRightClick = false, inGame = false, allegroWrite = false;
    mouseX = 0, mouseY = 0;
    lastKeyPress = 0, mouseWheel = 0;
    selectedBuildingId = BUILDING_CONNECTOR, selectedBuildingX = -1, selectedBuildingY = -1;

    allegroString.clear();

    rakIpAdress.clear(), rakClientName.clear();
    rakPort = 0;

    rakPeer = RakNet::RakPeerInterface::GetInstance();

    loadMapArray();

    mapArrayHeight = maxMapArrayHeight/5;
    mapArrayWidth = maxMapArrayWidth/5;

    al_start_timer(timer);
}

void Engine::cleanup(){
	// cleanup the all states

	while(!states.empty()){
		states.back()->cleanup();
		states.pop_back();
	}

	// switch back to windowed mode so other
	// programs won't get accidentally resized
	if(m_fullscreen){
		al_set_new_display_flags(ALLEGRO_WINDOWED);
	}

    al_destroy_font(defaultFont);
    al_destroy_font(smallFont);
    al_destroy_font(bigFont);
    al_destroy_bitmap(cursorImage);
    al_destroy_bitmap(groundImage1);
    al_destroy_bitmap(groundImage2);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
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

void Engine::draw(){
	// let the state update the game
	drawScreen = true;
	states.back()->draw(this);
}
