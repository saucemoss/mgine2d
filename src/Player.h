#pragma once
#include <iostream>
#include <fstream>
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
    PwrAttacking,
    Throwing,
    Hurting,
    InDialogue,
    AxeReclaim,
    MediPodSave,
    Dashing,
    WallGrabbing
};


class Player : public Entity, public Animated, public Collidable
{
public:
    Player(Vector2 pos);
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
    void SetAxeAttack();
    void KnockBack(Vector2 direction);
    void LoadAbilities();
    void SetDashing();
    void SetPwrAttack();
    void SetWallGrab();

    void Draw(int l) override;
    void DrawUI();
    void InitAnimations() override;

    //Movement Control
    bool is_touching_floor = false;
    bool is_standing_on_solid = false;
    bool left_wall_touch = false;
    bool right_wall_touch = false;
    bool left_grab = false;
    bool right_grab = false;
    bool looking_right = true;
    bool is_dying = false;
    bool taking_dmg = false;
    bool is_aiming = false;
    bool invincible = false;
    bool attack1 = true;
    bool visible = true;
    bool is_dashing = false;
    bool pwr_sensor_on = false;
    float invincible_counter;
    float invincible_time = 1.0f;
    float speed = 7.0f;
    float jump_force = 21.0f;
    float linear_dumping = 2.0f;
    float jump_add = 60.0f;
    float coyote_time = 0.15f;
    float coyote_time_counter = coyote_time;
    float jump_buffer_time = 0.10f;
    float jump_buffer_counter = jump_buffer_time;
    float m_ground_slam_vel = 10.0f;
    float face_turning_counter = 0.1f;
    float pad_sensitivity_threshold = 0.5f;
    float button_pressed_counter = 0.0f;
    float dashing_time = 0.20f;
    float dashing_counter = 0.0f;
    float wall_jump_time = 0.3f;
    float wall_jump_counter = 0.0f;
    bool wall_let_go = false;
    float wall_jump_hang_time = 2.0f;
    float wall_jump_hang_counter = 0.0f;
    b2Fixture* m_feet_sensor;
    b2Fixture* m_left_sensor;
    b2Fixture* m_right_sensor;
    b2Fixture* m_attack_sensor;
    b2Fixture* m_r_grab_sensor;
    b2Fixture* m_l_grab_sensor;
    b2Fixture* m_left_att_sensor;
    b2Fixture* m_right_att_sensor;
    b2Fixture* m_knockback_circle;
    b2Fixture* dash_att;
    b2Fixture* pwr_att;

    //Last safe pos
    float safe_pos_counter;
    Vector2 last_safe_pos;
    void UpdateSafePos(float dt);
    Vector2 new_pos;
    Vector2 old_pos;

    //States
    PlayerState state;
    bool finish_axe_reclaim_anim;

    //Weapons/skills
    b2Vec2 start_aim_pos;
    b2Vec2 axe_velocity;
    bool m_has_axe = false;
    bool axe_just_thrown = false;
    float axe_throw_pwr_counter = 0.0f;
    int axe_dmg = 35;
    static FireAxe* axe;
    Texture2D* axe_sprite;
    bool m_axe_unlocked = false;
    bool m_throwing_unlocked = false;
    bool m_power_attack_unlocked = false;
    bool m_axe_dash_unlocked = true;
    bool m_energy_unlocked = false;


    //HP
    int m_max_hp = 100;
    int current_hp = m_max_hp;
    
    //energy
    int m_max_energy = 100;
    int current_energy = m_max_energy;
    int axe_spawn_cost = 20;
    int min_axe_regen_cost = 5;
    int dash_cost = 6;
    int pwr_att_cost = 1;
    float energy_regen_counter;
    float energy_regen_interval = 4.0f;
    float spawn_counter;
    float spawn_interwal = 8.0f;
    bool WithdrawEnergy(int);
    void RegenEnergy(float dt);
    void GenerateOrbs(float dt);

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
    void SetThrowing();
    b2Vec2 GetTrajectoryPoint(b2Vec2& startingPosition, b2Vec2& startingVelocity, float n);
    void UpdateThrowingState(float dt);
    void UpdateAttackingState(float dt);
    void UpdatePowerAttackingState(float dt);
    void UpdateHurtingingState(float dt);
    void UpdateInDialogueState(float dt);
    void UpdateAxeReclaimState(float dt);
    void UpdateMediPodSaveState(float dt);
    void UpdateDashingState(float dt);
    void UpdateWallGrabbingState(float dt);
    



};

