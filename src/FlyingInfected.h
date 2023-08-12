#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collidable.h"
#include <map>
#include <box2d.h>
#include "Enemy.h"


enum class FlyingInfectedStates
{
    Idle,
    Flying,
    Attacking,
    Hurting,
    Dying
};


class FlyingInfected : public Enemy
{
public:
    FlyingInfected(const Rectangle& rectangle);
    ~FlyingInfected();

    virtual void Update(float dt) override;
    virtual void Die(int death_option) override;
    void CheckAgroSensor();
    virtual void TakeDmg(int dmg) override;
    virtual void InitAnimations() override;

    //custom sound
    Sound my_wingflap_sound;

    //Movement Control
    bool player_in_wingflap = false;
    float speed = 4.0f;
    float linear_dumping = 2.0f;
    b2Fixture* m_winghflap_sensor;

    //HP
    int m_max_hp = 150;
    int m_current_hp = m_max_hp;

    //States
    virtual void UpdateIdleState(float dt);
    void UpdateFlyingState(float dt);
    virtual void UpdateAttackingState(float dt);
    virtual void UpdateHurtingState(float dt);
    virtual void UpdateDyingState(float dt);

    // Inherited via Enemy
    virtual void UpdateRunningState(float dt);

};

