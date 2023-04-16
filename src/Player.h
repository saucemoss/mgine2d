#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include <box2d.h>

class Player : public Entity, public Animated
{
public:
    Player(b2World* physicsWorld);
    ~Player();
    
    void Update(float dt) override;
    void UpdatePosition();
    void PlayerMovementControl(float dt);
    void Draw() override;
    void InitAnimations() override;

   
    b2Body* body{};
    b2World* physicsWorld;

    //Movement Controll
    void set_velocity_x(float vx);
    void set_velocity_y(float vy);
    void set_velocity_xy(float vx, float vy);
    bool can_move_in_x_direction(bool moving_right);
    void check_if_on_floor();
    void check_if_jump();
    void check_if_move();
    bool is_touching_floor = true;
    bool looking_right = true;
};
