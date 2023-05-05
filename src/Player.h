#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collisions.h"

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
    void SyncColliders();
    void Draw() override;
    void DrawCollider() override;
    void InitAnimations() override;

    //Movement Controll
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
    float jump_buffer_time = 0.04f;
    float jump_buffer_counter = jump_buffer_time;
    Rectangle feetSensor;

    //States
    PlayerState state;

    void UpdateIdleState(float dt);
    void UpdateRunningState(float dt);
    void UpdateJumpingState(float dt);
    void UpdateFallingState(float dt);
};



