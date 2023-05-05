#pragma once

#include "LDtkLoader/Project.hpp"
#include "LDtkLoader/World.hpp"
#include "SolidTile.h"
#include <raylib.h>
#include "LevelPortal.h"
#include "Entity.h"


class LevelManager
{
public:
    LevelManager();
    ~LevelManager();
    void LoadLevel(std::string level_name);
    void UnloadLevel();
    void Draw();

    const ldtk::Project* ldtkProject{};
    const ldtk::World* ldtkWorld{};
    const ldtk::Level* currentLdtkLevel{};
    const ldtk::Layer* bgLayer;
    Texture2D currentTilesetTexture;
    Texture2D renderedLevelTexture;
    RenderTexture2D renderTexture;
    Texture2D backgroundTexture;


    std::vector<SolidTile*> solid_tiles;
    std::vector<Entity*> level_entities;

};