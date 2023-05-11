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
    void Update(float dt);

    const ldtk::Project* ldtkProject{};
    const ldtk::World* ldtkWorld{};
    const ldtk::Level* currentLdtkLevel{};
    const ldtk::Layer* bgLayer;
    ldtk::IntPoint levelSize;

    Texture2D terrainTilesetTexture;
    Texture2D bgTilesetTexture;
    Texture2D currentTilesetTexture;
    Texture2D renderedLevelTexture;
    Texture2D renderedBGTexture;
    Texture2D backgroundTexture;
    RenderTexture2D renderTexture;
    RenderTexture2D renderBGTexture;
    


    std::vector<SolidTile*> solid_tiles;
    std::vector<Entity*> level_entities;
    std::vector<LevelPortal*> level_portals;

};