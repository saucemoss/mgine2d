#pragma once

#include <raylib.h>
#include "BaseScreen.h"
#include <box2d.h>


class GameScreen : public BaseScreen
{
public:
    GameScreen();
    ~GameScreen();

    void Draw() override;
    Screens Update(float dt) override;

    static b2World* b2world;

};
