#pragma once

#include "BaseScreen.h"
#include <raylib.h>

class MenuScreen : public BaseScreen
{
private:
    enum class MenuOption {
        Continue,
        NewGame,
        Options,
        Exit
    };

    MenuOption selectedOption;
    int optionIndex;

public:
    MenuScreen();

    void Draw() override;
    Screens Update(float dt) override;
};
