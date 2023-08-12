#pragma once
#include <raylib.h>
#include "BaseScreen.h"
#include "LevelManager.h"
#include "Player.h"
#include "SoundManager.h"
#include "Particles.h"
#include "Shaders.h"

class GameScreen : public BaseScreen
{
public:
    GameScreen();
    ~GameScreen();

    void UpdateCamera(float dt);

    Screens Update(float dt) override;
    static Camera2D camera;
    static Camera2D player_focused_cam;
    static Camera2D shake_cam;
   
    static Player* player;
    static LevelManager* LevelMgr;
    static SoundManager* SoundMgr;
    static ParticlesManager* Particles;
    static Shaders* shaders;

    static bool debug;
    void Draw() override;
    void DebugShapes();
    static void add_trauma(float intensity);

    Texture2D vignette;
    void DrawVignette();
private:
    static float shake;
    static float trauma;
    float max_cam_rotation = 0.2f;
    float max_cam_x_offset = 8.0f;
    float max_cam_y_offset = 8.0f;
};
