#pragma once

#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collidable.h"
#include <map>
#include <box2d.h>


enum class EnemyState
{
    Idle,
    Spawning,
    Running,
    Flying,
    Attacking,
    Hurting,
    Dying,
    Patrolling
};


class Enemy : public Entity, public Animated, public Collidable
{
public:
    Enemy(const Rectangle& rectangle, ColliderTag tag);
    virtual void Die(int death_option) = 0;
    void CheckTouchGround();
    virtual void TakeDmg(int dmg) = 0;
    void bleed_particles();
    void set_velocity_x(float vx);
    void set_velocity_y(float vy);
    void set_velocity_xy(float vx, float vy);
    void Draw(int l) override;

    //Movement Control
    bool taking_dmg = false;
    bool player_in_agro = false;
    bool is_touching_floor = false;
    bool left_player_touch = false;
    bool right_player_touch = false;
    bool looking_right = true;
    bool player_in_dmg_zone = false;
    float speed = 5.0f;
    float linear_dumping = 2.0f;
    float dmg_counter = 0.0f;
    int solid_contacts = 0;
    int ground_contacts = 0;
    b2Fixture* m_feet_sensor;
    b2Fixture* m_left_sensor;
    b2Fixture* m_right_sensor;
    b2Fixture* m_agro_sensor;
    b2Fixture* m_attack_sensor;
    b2Fixture* m_proximity_sensor;

    //HP
    int m_max_hp;
    int m_current_hp = m_max_hp;

    //Drawing
    bool custom_pos = false;
    float spritePosX = 0.0f;
    float spritePosY = 0.0f;
    //Custom sprite positions
    Vector2 sprite_offset_32;
    Vector2 sprite_offset_96;
    Vector2 sprite_offset_224;


    //States
    EnemyState state;
    virtual void UpdateIdleState(float dt) = 0;
    virtual void UpdateRunningState(float dt) = 0;
    virtual void UpdateAttackingState(float dt) = 0;
    virtual void UpdateHurtingState(float dt) = 0;
    virtual void UpdateDyingState(float dt) = 0;

    //Debug
    std::map<EnemyState, std::string> StatesStrMap{};
    std::map<ColliderTag, std::string> ColStrMap{};
    std::string contact_debug_str;



};

