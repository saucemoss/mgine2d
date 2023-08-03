#pragma once
#include <raylib.h>
#include "BaseScreen.h"
#include "LevelManager.h"
#include "Player.h"
#include "SoundManager.h"
#include "Particles.h"


class GameScreen : public BaseScreen
{
public:
    GameScreen();
    ~GameScreen();

    void UpdateCamera(float dt);

    Screens Update(float dt) override;
    static Camera2D camera;
    static Camera2D player_focused_cam;
   
    static Player* player;
    static LevelManager* LevelMgr;
    static SoundManager* SoundMgr;
    static ParticlesManager* Particles;
    static bool debug;
    void Draw() override;

    void DebugShapes();

};
