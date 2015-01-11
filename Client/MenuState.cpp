#include "Engine.h"
#include "MenuState.h"
#include "GameState.h"
#include "PlayState.h"

MenuState MenuState::m_MenuState;

void MenuState::init(){
    int i = (inGame) ? 0 : 1;
    int loopValue = 4;
    for(; i < loopValue; i++){
        float x = 0, y = 0, width = 0, height = 0;
        int buttonId = i;

        width = 256, height = 64;

        x = screenWidth/2-width/2, y = screenHeight/2-height/2-height*(((inGame) ? loopValue-1.0 : loopValue+0.0)/2)+height*i;

        Button *newButton = new Button();
        newButton->setPos(x, y);
        newButton->setDimensions(width, height);
        newButton->setTypeId(buttonId);

        addButtonToList(newButton);
    }

    menuId = 0, menuIdCopy = 0, rakPort = 0;
    lanMode = false;
    rakIpAdress.clear(), rakClientName.clear();
}
void MenuState::cleanup(){
    for(int i = 0; i < MAX_BUTTONS; i++)
        buttonList[i] = NULL;
    for(int i = 0; i < MAX_INPUT_FIELDS; i++)
        inputFieldList[i] = NULL;
    inGame = false;
}

void MenuState::pause(){

}
void MenuState::resume(){

}

void MenuState::update(Engine* engine){

    al_get_mouse_state(&mouseState);

    al_get_keyboard_state(&keyState);

    if(menuId != menuIdCopy){
        for(int i = 0; i < MAX_BUTTONS; i++)
            buttonList[i] = NULL;

        for(int i = 0; i < MAX_INPUT_FIELDS; i++)
            inputFieldList[i] = NULL;

        if(menuId == 0){
            int i = (inGame) ? 0 : 1;
            int loopValue = 4;
            for(; i < loopValue; i++){
                float x = 0, y = 0, width = 0, height = 0;
                int buttonId = i;

                width = 256, height = 64;

                x = screenWidth/2-width/2, y = screenHeight/2-height/2-height*(((inGame) ? loopValue-1.0 : loopValue+0.0)/2)+height*i;

                Button *newButton = new Button();
                newButton->setPos(x, y);
                newButton->setDimensions(width, height);
                newButton->setTypeId(buttonId);

                addButtonToList(newButton);
            }
        }else if(menuId == 1){
            if(!lanMode){
                float x = 0, y = 0, width = 0, height = 0;
                int buttonId = 4;

                width = 256, height = 64;

                x = screenWidth/2-width/2, y = screenHeight/2-height/2+height;

                Button *newButton = new Button();
                newButton->setPos(x, y);
                newButton->setDimensions(width, height);
                newButton->setTypeId(buttonId);

                addButtonToList(newButton);

                x = 0, y = 0, width = 0, height = 0;
                int inputFieldId = 0;

                width = 512, height = 32;

                x = screenWidth/2-width/2, y = screenHeight/2-height/4;

                InputField *newField = new InputField();
                newField->setPos(x, y);
                newField->setDimensions(width, height);
                newField->setTypeId(inputFieldId);
                newField->setSelected(true);

                addInputFieldToList(newField);
            }else{
                float x = 0, y = 0, width = 0, height = 0;
                int buttonId = 5;

                width = 256, height = 64;

                x = screenWidth/2-width/2, y = screenHeight/2-height/2+height;

                Button *newButton = new Button();
                newButton->setPos(x, y);
                newButton->setDimensions(width, height);
                newButton->setTypeId(buttonId);

                addButtonToList(newButton);

                x = 0, y = 0, width = 0, height = 0;
                int inputFieldId = 1;

                width = 512, height = 32;

                x = screenWidth/2-width/2, y = screenHeight/2-height/4;

                InputField *newField = new InputField();
                newField->setPos(x, y);
                newField->setDimensions(width, height);
                newField->setTypeId(inputFieldId);
                newField->setSelected(true);

                addInputFieldToList(newField);
            }

        }else if(menuId == 2){
            float x = 0, y = 0, width = 0, height = 0;
            int buttonId = 5;

            width = 256, height = 64;

            x = screenWidth/2-width/2, y = screenHeight/2-height/2+height;

            Button *newButton = new Button();
            newButton->setPos(x, y);
            newButton->setDimensions(width, height);
            newButton->setTypeId(buttonId);

            addButtonToList(newButton);

            x = 0, y = 0, width = 0, height = 0;
            int inputFieldId = 1;

            width = 512, height = 32;

            x = screenWidth/2-width/2, y = screenHeight/2-height/4;

            InputField *newField = new InputField();
            newField->setPos(x, y);
            newField->setDimensions(width, height);
            newField->setTypeId(inputFieldId);
            newField->setSelected(true);

            addInputFieldToList(newField);
        }
    }
    menuIdCopy = menuId;

    //Input +
    if(al_key_down(&keyState, ALLEGRO_KEY_ESCAPE)){
        if(lastKeyPress != ALLEGRO_KEY_ESCAPE){
            if(inGame){
                engine->popState();
            }else if(menuId == 0){
                engine->quit();
            }else{
                menuId = 0;
            }
            lastKeyPress = ALLEGRO_KEY_ESCAPE;
        }
     }else if(al_key_down(&keyState, ALLEGRO_KEY_ENTER)){
        if(lastKeyPress != ALLEGRO_KEY_ENTER){
            if(menuId == 0){
                menuId = 1;
            }else if(menuId == 1){
                if(!lanMode){
                    menuId = 2;
                    for(int i = 0; i < MAX_INPUT_FIELDS; i++){
                        if(inputFieldList[i] != NULL){
                            if(inputFieldList[i]->getTypeId() == 0){
                                if(inputFieldList[i]->getInput().size() > 0){
                                    rakIpAdress = inputFieldList[i]->getInput();
                                }else{
                                    menuId = 1;
                                    inputFieldList[i]->clearInput();
                                }
                            }
                        }
                    }
                }else{
                    for(int i = 0; i < MAX_INPUT_FIELDS; i++){
                        if(inputFieldList[i] != NULL){
                            if(inputFieldList[i]->getTypeId() == 1){
                                std::string tempPort = inputFieldList[i]->getInput();
                                if(std::all_of(tempPort.begin(), tempPort.end(), ::isdigit) && tempPort.size() > 0){
                                    rakPort = atoi(inputFieldList[i]->getInput().c_str());
                                    engine->changeState(PlayState::instance());
                                }else{
                                    menuId = 1;
                                    inputFieldList[i]->clearInput();
                                }
                            }
                        }
                    }
                }
            }else if(menuId == 2){
                for(int i = 0; i < MAX_INPUT_FIELDS; i++){
                    if(inputFieldList[i] != NULL){
                        if(inputFieldList[i]->getTypeId() == 1){
                            std::string tempPort = inputFieldList[i]->getInput();
                            if(std::all_of(tempPort.begin(), tempPort.end(), ::isdigit) && tempPort.size() > 0){
                                rakPort = atoi(inputFieldList[i]->getInput().c_str());
                                engine->changeState(PlayState::instance());
                            }else{
                                menuId = 1;
                                inputFieldList[i]->clearInput();
                            }
                        }
                    }
                }
            }
            lastKeyPress = ALLEGRO_KEY_ENTER;
        }
     }
    //Input -

    //Update Entities +
    for(int i = 0; i < MAX_BUTTONS; i++){
        if(buttonList[i] != NULL){
            buttonList[i]->update();
            if(buttonList[i]->clicked){
                switch(buttonList[i]->typeId){
                    case 0:
                        ///Resume
                        engine->popState();
                        break;

                    case 1:
                        ///Online
                        if(inGame){
                            rakPeer->Shutdown(300);
                            inGame = false;
                        }
                        menuId = 1;
                        break;

                    case 2:
                        ///LAN
                        if(inGame){
                            rakPeer->Shutdown(300);
                            inGame = false;
                        }
                        menuId = 1;
                        lanMode = false;
                        break;

                    case 3:
                        ///Quit
                        if(inGame){
                            rakPeer->Shutdown(300);
                            inGame = false;
                        }
                        engine->quit();
                        break;

                    case 4:
                        ///Next menuId
                        menuId = 2;
                        for(int i = 0; i < MAX_INPUT_FIELDS; i++){
                            if(inputFieldList[i] != NULL){
                                if(inputFieldList[i]->getTypeId() == 0){
                                    if(inputFieldList[i]->getInput().size() > 0){
                                        rakIpAdress = inputFieldList[i]->getInput();
                                    }else{
                                        menuId = 1;
                                        inputFieldList[i]->clearInput();
                                    }
                                }
                            }
                        }

                        break;

                    case 5:
                        ///Join server
                        {
                            if(!lanMode){
                                for(int i = 0; i < MAX_INPUT_FIELDS; i++){
                                    if(inputFieldList[i] != NULL){
                                        if(inputFieldList[i]->getTypeId() == 1){
                                            std::string tempPort = inputFieldList[i]->getInput();
                                            if(std::all_of(tempPort.begin(), tempPort.end(), ::isdigit) && tempPort.size() > 0){
                                                rakPort = atoi(tempPort.c_str());
                                                menuId = 0;
                                                engine->changeState(PlayState::instance());
                                            }else{
                                                inputFieldList[i]->clearInput();
                                            }
                                        }
                                    }
                                }
                            }else{
                                for(int i = 0; i < MAX_INPUT_FIELDS; i++){
                                    if(inputFieldList[i] != NULL){
                                        if(inputFieldList[i]->getTypeId() == 1){
                                            std::string tempPort = inputFieldList[i]->getInput();
                                            if(std::all_of(tempPort.begin(), tempPort.end(), ::isdigit) && tempPort.size() > 0){
                                                rakPort = atoi(inputFieldList[i]->getInput().c_str());
                                                engine->changeState(PlayState::instance());
                                            }else{
                                                menuId = 1;
                                                inputFieldList[i]->clearInput();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        break;
                }
            }
        }
    }

    for(int i = 0; i < MAX_INPUT_FIELDS; i++){
        if(inputFieldList[i] != NULL){
            inputFieldList[i]->update();
        }
    }
    //Update Entities -

    //Rest +

    //Rest -
}

void MenuState::draw(Engine* engine){

    if(menuId != 0)
        al_draw_text(defaultFont, al_map_rgb(150, 150, 150), screenWidth/2, screenHeight/2-32, ALLEGRO_ALIGN_CENTRE, (menuId == 1) ? ((lanMode) ? "Please write in the port of the LAN server" : "Please write in the IP adress of the server") : "Please write in which port to use, default is 2020");

    for(int i = 0; i < MAX_BUTTONS; i++){
        if(buttonList[i] != NULL){
            buttonList[i]->draw();
        }
    }

    for(int i = 0; i < MAX_INPUT_FIELDS; i++){
        if(inputFieldList[i] != NULL){
            inputFieldList[i]->draw();
        }
    }
}
