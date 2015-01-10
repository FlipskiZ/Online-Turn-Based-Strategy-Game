#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include "Entity.h"

class InputField : public Entity{
    public:
        InputField(void);
        void draw();
        void update();

        void setSelected(bool selected);
        void clearInput();

        std::string getInput();

        bool selected;

    protected:

    private:
        std::string inputText;

        bool cursorAppear;
        int cursorAppearHelper;
};

#endif // INPUTFIELD_H
