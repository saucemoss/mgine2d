#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collidable.h"
#include <map>
#include <box2d.h>


enum class FlyingInfectedStates
{
    Idle,
    Flying,
    Attacking,
    Hurting,
    Dying
};


class FlyingInfected : public Entity, public Animated, public Collidable
{
public:
    FlyingInfected(const Rectangle& rectangle);
    ~FlyingInfected();

    void Update(float dt) override;
    void Die(int death_option);
    void CheckAgroSensor();
    void CheckTouchGround();
    void CheckPlayerTouch();
    void TakeDmg(int dmg);
    void set_velocity_x(float vx);
    void set_velocity_y(float vy);
    void set_velocity_xy(float vx, float vy);

    void Draw(int l) override;
    void InitAnimations() override;

    //Movement Control
    bool player_in_agro = false;
    bool is_touching_floor = false;
    bool left_player_touch = false;
    bool right_player_touch = false;
    bool looking_right = true;
    bool player_in_dmg_zone = false;
    float speed = 4.0f;
    float linear_dumping = 2.0f;
    int solid_contacts = 0;
    int ground_contacts = 0;
    b2Fixture* m_left_sensor;
    b2Fixture* m_right_sensor;
    b2Fixture* m_agro_sensor;
    b2Fixture* m_attack_sensor;


    //HP
    int m_max_hp = 150;
    int m_current_hp = m_max_hp;


    //States
    FlyingInfectedStates state;
    void UpdateIdleState(float dt);
    void UpdateFlyingState(float dt);
    void UpdateAttackingState(float dt);
    void UpdateHurtingState(float dt);
    void UpdateDyingState(float dt);

    //Debug
    std::map<FlyingInfectedStates, std::string> StatesStrMap{};
    std::map<ColliderTag, std::string> ColStrMap{};
    std::string contact_debug_str;
    std::string axe_vel_str;
    float axe_vel;




    Shader shdrOutline;

    float outlineSize = 1.0f;
    float outlineColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };      // Normalized RED color 
    float textureSize[2] = { 192.0f,160.0f };

    int outlineSizeLoc;
    int outlineColorLoc;
    int textureSizeLoc;

};

