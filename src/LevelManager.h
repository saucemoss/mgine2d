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
#include "Particles.h"



class LevelManager
{
public:
    LevelManager(bool is_new_game, int save_file_num);
    ~LevelManager();
    void LoadLevel(std::string level_name);
    void SaveLevel();
    void LoadSavedLevel();
    void UnloadLevel();
    void ResetLevel();
    void Draw();
    void Update(float dt);
    static void RemoveEntityFromLevel(Entity& e);

    //saving system
    int save_file_num;

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
    //decors in front of player
    RenderTexture2D decorationRenderTextureFront;
    Texture2D decorationRenderedLevelTextureFront;
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
    
    
    //perlin noise 
    Texture2D perlin;
    bool level_fog_on = false;


    //Layer draw
    void DrawForeGround();
    void DrawInFrontOfPlayer();
    float layer_scroll = 0.0f;

    //Level Particles
    ParticleEmitter* level_ambient_particles;
    ParticleEmitter* level_particles_foreground;
    bool level_particles_on;
    
    //Object containers
    static std::vector<std::unique_ptr<Collidable>> solid_tiles;
    static std::vector<std::unique_ptr<Entity>> level_entities_safe;
    static std::vector<Entity*> queue_entities;



};

