#pragma once
#include <raylib.h>
#include "BaseScreen.h"
#include "Player.h"
#include "LevelManager.h"


class GameScreen : public BaseScreen
{
public:
    GameScreen();
    ~GameScreen();

    void UpdateCamera(float dt);

    Screens Update(float dt) override;
    static Camera2D camera;
   
    static Player* player;
    static LevelManager* LevelMgr;
    static bool debug;
    void Draw() override;
};
