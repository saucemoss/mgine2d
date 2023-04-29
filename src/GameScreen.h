#pragma once
#include <raylib.h>
#include "BaseScreen.h"
#include <box2d.h>
#include "LDtkLoader/Project.hpp"


class GameScreen : public BaseScreen
{
public:
    GameScreen();
    ~GameScreen();

    void Draw() override;
    Screens Update(float dt) override;
    static Camera2D camera;
    ldtk::Project *ldtkProject{};
    const ldtk::World *ldtkWorld{};
    const ldtk::Level *currentLdtkLevel{};
    const ldtk::Layer *bgLayer;
    Texture2D currentTilesetTexture;
    Texture2D renderedLevelTexture;

};
