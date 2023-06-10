#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collidable.h"
#include <map>
#include <box2d.h>


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
    void NewBody();
    ~Player();
    
    void Update(float dt) override;
    void CheckTouchGround();
    void CheckWallTouch();
    void set_velocity_x(float vx);
    void set_velocity_y(float vy);
    void set_velocity_xy(float vx, float vy);
    void LevelPortalCheck();

    void Draw() override;
    void InitAnimations() override;

    //Movement Control
    bool is_touching_floor = false;
    bool left_wall_touch = false;
    bool right_wall_touch = false;
    bool looking_right = true;
    float speed = 7.0f;
    float jump_force = 21.0f;
    float linear_dumping = 2.0f;
    float jump_add = 1.0f;
    float coyote_time = 0.15f;
    float coyote_time_counter = coyote_time;
    float jump_buffer_time = 0.10f;
    float jump_buffer_counter = jump_buffer_time;
    float m_ground_slam_vel = 10.0f;
    b2Fixture* m_feet_sensor;
    b2Fixture* m_left_sensor;
    b2Fixture* m_right_sensor;

    //States
    PlayerState state;
    

    //Debug
    std::map<PlayerState, std::string> StatesStrMap{};
    std::map<ColliderTag, std::string> ColStrMap{};
    std::string contact_debug_str;


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

