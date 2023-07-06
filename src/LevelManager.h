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

// Custom Blend Modes
#define RLGL_SRC_ALPHA 0x0302
#define RLGL_MIN 0x8007
#define RLGL_MAX 0x8008

#define MAX_BOXES     2000
#define MAX_SHADOWS   MAX_BOXES*3         // MAX_BOXES *3. Each box can cast up to two shadow volumes for the edges it is away from, and one for the box itself
#define MAX_LIGHTS    32


// Shadow geometry type
typedef struct ShadowGeometry {
    Vector2 vertices[4];
} ShadowGeometry;

// Light info type
typedef struct LightInfo {
    bool active;                // Is this light slot active?
    bool dirty;                 // Does this light need to be updated?
    bool valid;                 // Is this light in a valid position?

    Vector2 position;           // Light position
    RenderTexture mask;         // Alpha mask for the light
    float outerRadius;          // The distance the light touches
    Rectangle bounds;           // A cached rectangle of the light bounds to help with culling

    ShadowGeometry shadows[MAX_SHADOWS];
    int shadowCount;
} LightInfo;

class LevelManager
{
public:
    LevelManager();
    ~LevelManager();
    void LoadLevel(std::string level_name);
    void UnloadLevel();
    void Draw();
    void Update(float dt);


    //ldtk stuff
    const ldtk::Project* ldtkProject{};
    const ldtk::World* ldtkWorld{};
    const ldtk::Level* currentLdtkLevel{};
    const ldtk::Layer* bgLayer;
    ldtk::IntPoint levelSize;

    //light methods
    void SetupLight(float x, float y, float radius);
    void MoveLight(LightInfo& light, float x, float y);
    void ComputeShadowVolumeForEdge(LightInfo& light, Vector2 sp, Vector2 ep);
    void DrawLightMask(LightInfo& light);
    static void RemoveEntityFromLevel(Entity& e);
    bool UpdateLight(LightInfo& light, std::vector<Rectangle*> m_light_walls);
    void SetupBoxes();
    //m_lights
    void DrawSpotLights();
    int nextLight = 1;
    bool showLines = false;
    RenderTexture lightMask;
    std::vector<LightInfo> m_lights;
    std::vector<Rectangle*> m_light_walls;
    float m_darkness_strength;

    //physics
    b2Vec2 gravity = b2Vec2(0.0f, 80.0f);
    static bool CheckPlayerInSensor(b2Fixture& sensor);
    void SolidTilesToBigBoxes();
    ContactListener* contacts;
  
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
    //paralaxed bg
    Texture2D paralaxedBackgroundSpriteAtlas;
    RenderTexture2D paralaxBackgroundRenderTexture;
    Texture2D paralaxedBackgroundRenderedLevelTexture;

    //Layer draw
    void DrawForeGround();
    

    //Object containers
    std::vector<std::unique_ptr<Collidable>> solid_tiles;
    static std::vector<std::unique_ptr<Entity>> level_entities_safe;

    //Box2d
    static b2World* world;

};

