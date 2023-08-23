#pragma once

#include <raylib.h>
#include "BaseScreen.h"

class OptionsScreen : public BaseScreen
{

public:
    OptionsScreen();
    void Draw() override;
    Screens Update(float dt) override;
};

