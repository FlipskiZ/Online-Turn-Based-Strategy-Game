#include "Engine.h"
#include "InputField.h"

InputField::InputField(){
    this->inputText.clear();
}

void InputField::setSelected(bool selected){
    this->selected = selected;
    allegroWrite = true;
    allegroString = this->inputText;
    this->cursorAppear = true;
    this->cursorAppearHelper = 0;
}

void InputField::clearInput(){
    this->inputText.clear();
    if(this->selected)
        allegroString.clear();
}

std::string InputField::getInput(){
    return this->inputText;
}

void InputField::draw(){
    al_draw_rectangle(this->posX+1.25, this->posY+1.25, this->posX+this->width-1.25, this->posY+this->height-1.25, (!this->selected) ? al_map_rgb(155,155,155) : al_map_rgb(215,215,215), 2.5);

    if(this->typeId != 3)
        al_draw_textf(defaultFont, (!this->selected) ? al_map_rgb(150,150,150) : al_map_rgb(200,200,200), this->posX+this->width/2, this->posY+(this->height-al_get_font_line_height(defaultFont))/2, ALLEGRO_ALIGN_CENTRE, (!this->selected || !this->cursorAppear) ? this->inputText.c_str() : "%s|", this->inputText.c_str());
    else
        al_draw_textf(smallFont, (!this->selected) ? al_map_rgb(150,150,150) : al_map_rgb(200,200,200), this->posX+12, this->posY+(this->height-al_get_font_line_height(smallFont))/2, ALLEGRO_ALIGN_LEFT, (!this->selected || !this->cursorAppear) ? this->inputText.c_str() : "%s|", this->inputText.c_str());

    /**InputFieldID is used as follows: 0 - IP-Adress  ---  1 - Port  ---  2 - Client Name --- 3 - Chat --- 4 - Nothing**/
}

void InputField::update(){
    if(mouseButtonLeftClick){
        if(checkCollision(this->posX, this->posY, mouseX, mouseY, this->width, this->height, 0, 0)){
            this->selected = true;
            allegroWrite = true;
            allegroString = this->inputText;
        }else{
            this->selected = false;
            this->cursorAppear = false;
            this->cursorAppearHelper = 0;
            allegroWrite = false;
            allegroString.clear();
        }
    }

    if(this->selected){
        this->inputText = allegroString;
        if(al_get_text_width((this->typeId == 3) ? smallFont : defaultFont, allegroString.c_str()) > this->width-24 && allegroString.size() > 0){
            allegroString.pop_back();
            this->inputText.pop_back();
        }
    }

    if(this->cursorAppearHelper >= 30){
        this->cursorAppear = !this->cursorAppear;
        this->cursorAppearHelper = 0;
    }
    if(this->selected)
        this->cursorAppearHelper++;
}
