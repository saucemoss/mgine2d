#pragma once
#include <box2d.h>
#include "LDtkLoader/Project.hpp"
#include "LDtkLoader/World.hpp"
#include "Collidable.h"
#include "SolidTile.h"
#include <raylib.h>
#include "LevelPortal.h"
#include "Entity.h"
#include "Door.h"
#include "FireAxe.h"
#include "ContactListener.h"
#include "LightManager.h"



class LevelManager
{
public:
    LevelManager();
    ~LevelManager();
    void LoadLevel(std::string level_name);
    void UnloadLevel();
    void Draw();
    void Update(float dt);
    static void RemoveEntityFromLevel(Entity& e);

    //lighting stuff
    static float m_darkness_strength;
    LightManager* lights;

    //ldtk stuff
    const ldtk::Project* ldtkProject{};
    const ldtk::World* ldtkWorld{};
    static const ldtk::Level* currentLdtkLevel;
    const ldtk::Layer* bgLayer;
    ldtk::IntPoint levelSize;
    std::string next_level;
    Vector2 new_player_pos;

    //physics
    static b2World* world;
    b2Vec2 gravity = b2Vec2(0.0f, 80.0f);
    void SolidTilesToBigBoxes();
    ContactListener* contacts;
    DestructionListener* destruction_listener;
    ContactFilter* contact_filter;
  
    //Textures definition
    //static background
    Texture2D baseBackgroundSpriteAtlas;
    RenderTexture2D baseBackgroundRenderTexture;
    Texture2D baseBackgroundRenderedLevelTexture;
    //decors,platforms
    Texture2D decorationSpriteAtlas;
    RenderTexture2D decorationRenderTexture;
    Texture2D decorationRenderedLevelTexture;
    //solids
    Texture2D laboratorySolidsSpriteAtlas;
    RenderTexture2D laboratorySolidsRenderTexture;
    Texture2D laboratorySolidsRenderedLevelTexture;
    //paralaxed foreground
    Texture2D paralaxedForegroundSpriteAtlas;
    RenderTexture2D paralaxedForegroundRenderTexture;
    Texture2D paralaxedForegroundRenderedLevelTexture;
    //paralaxed bg
    Texture2D paralaxedBackgroundSpriteAtlas;
    RenderTexture2D paralaxBackgroundRenderTexture;
    Texture2D paralaxedBackgroundRenderedLevelTexture;

    //Layer draw
    void DrawForeGround();
    
    //Object containers
    static std::vector<std::unique_ptr<Collidable>> solid_tiles;
    static std::vector<std::unique_ptr<Entity>> level_entities_safe;
    static std::vector<Entity*> queue_entities;



};

