#pragma once

#include "LDtkLoader/Project.hpp"
#include "LDtkLoader/World.hpp"
#include "SolidTile.h"
#include <raylib.h>
#include "LevelPortal.h"
#include "Entity.h"
#include "Door.h"


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
    Texture2D laboratoryTilesetTexture;
    Texture2D DecorTex;

    RenderTexture2D staticBGRenderTexture;
    Texture2D staticRenderedBGTexture;

    RenderTexture2D staticDecorRenderTexture;
    Texture2D staticDecorTexture;


    RenderTexture2D renderTexture;
    RenderTexture2D renderBGTexture;
    


    std::vector< std::unique_ptr<SolidTile>> solid_tiles;
    std::vector<std::unique_ptr<Entity>> level_entities_safe;
    std::vector<std::unique_ptr<Door>> level_doors;
    std::vector<LevelPortal*> level_portals;

};