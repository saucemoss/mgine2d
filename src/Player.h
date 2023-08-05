#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collidable.h"
#include <map>
#include <box2d.h>
#include "FireAxe.h"


enum class PlayerState
{
    Idle,
    Running,
    Jumping,
    Falling,
    Sliding,
    Dying,
    Attacking,
    Throwing,
    Hurting
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
    void take_dmg(int dmg);
    void Die();
    void SetRandomAxeAttack();

    void Draw(int l) override;
    void DrawUI();
    void InitAnimations() override;

    //Movement Control
    bool is_touching_floor = false;
    bool left_wall_touch = false;
    bool right_wall_touch = false;
    bool looking_right = true;
    bool is_dying = false;
    bool taking_dmg = false;
    bool is_aiming = false;
    bool invincible = false;
    float invincible_counter;
    float invincible_time = 1.0f;
    float speed = 7.0f;
    float jump_force = 21.0f;
    float linear_dumping = 2.0f;
    float jump_add = 1.0f;
    float coyote_time = 0.15f;
    float coyote_time_counter = coyote_time;
    float jump_buffer_time = 0.10f;
    float jump_buffer_counter = jump_buffer_time;
    float m_ground_slam_vel = 10.0f;
    float face_turning_counter = 0.1f;
    float pad_sensitivity_threshold = 0.5f;
    b2Fixture* m_feet_sensor;
    b2Fixture* m_left_sensor;
    b2Fixture* m_right_sensor;
    b2Fixture* m_attack_sensor;
    b2Fixture* m_left_att_sensor;
    b2Fixture* m_right_att_sensor;
    b2Fixture* m_knockback_circle;

    //States
    PlayerState state;

    //Weapons
    b2Vec2 start_aim_pos;
    b2Vec2 axe_velocity;
    bool m_has_axe = false;
    bool axe_just_thrown = false;
    float axe_throw_pwr_counter = 0.0f;
    int axe_dmg = 35;
    static FireAxe* axe;
    Texture2D* axe_sprite;

    //HP
    int m_max_hp = 100;
    int current_hp = m_max_hp;
    

    //Debug
    std::map<PlayerState, std::string> StatesStrMap{};
    std::map<ColliderTag, std::string> ColStrMap{};
    std::string contact_debug_str;


    void UpdateIdleState(float dt);
    void Jump();
    void UpdateRunningState(float dt);
    void UpdateJumpingState(float dt);
    void UpdateFallingState(float dt);
    void UpdateDyingState(float dt);
    void UpdateSlidingState(float dt);
    b2Vec2 GetTrajectoryPoint(b2Vec2& startingPosition, b2Vec2& startingVelocity, float n);
    void UpdateThrowingState(float dt);
    void UpdateAttackingState(float dt);
    void UpdateHurtingingState(float dt);



};

