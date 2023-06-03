#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collidable.h"
#include <map>


enum class PlayerState
{
    Idle,
    Running,
    Jumping,
    Falling
};


class Player : public Entity, public Animated, public Collidable
{
public:
    Player();
    ~Player();
    
    void Update(float dt) override;
    void LevelPortalCheck();
    void MovingBlockCheck(float dt);
    void SyncColliders();
    void Draw() override;
    virtual void DrawCollider() override;
    void InitAnimations() override;

    //Movement Control
    void ApplyForces(float dt);
    void TransformPos(Vector2 pos);
    bool is_touching_floor = false;
    bool looking_right = true;
    float speed = 300.0f;
    float acceleration = 1000.0f;
    float friction = 1800.0f;
    float gravity = 60.0f;
    float jump_force = 700.0f;
    float coyote_time = 0.15f;
    float coyote_time_counter = coyote_time;
    float jump_buffer_time = 0.10f;
    float jump_buffer_counter = jump_buffer_time;
    Rectangle feetSensor;

    //States
    PlayerState state;
    std::map<PlayerState, std::string> StatesStrMap{};

    void UpdateIdleState(float dt);
    void UpdateRunningState(float dt);
    void UpdateJumpingState(float dt);
    void UpdateFallingState(float dt);





    Shader shdrOutline;

    float outlineSize = 1.0f;
    float outlineColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };      // Normalized RED color 
    float textureSize[2] = { 192.0f,160.0f };

    int outlineSizeLoc;
    int outlineColorLoc;
    int textureSizeLoc;
};



