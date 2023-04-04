#pragma once

#include <raylib.h>
#include "BaseScreen.h"


class GameScreen : public BaseScreen
{
public:
    GameScreen();
    ~GameScreen();

    void Draw() override;
    Screens Update(float dt) override;

};
