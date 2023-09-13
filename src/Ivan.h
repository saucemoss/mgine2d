#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collidable.h"
#include <map>
#include <box2d.h>
#include "Enemy.h"



class Ivan : public Enemy
{
public:
    Ivan(const Rectangle& rectangle, const ldtk::ArrayField<ldtk::IntPoint> path_points);
    ~Ivan();

    const ldtk::ArrayField<ldtk::IntPoint> m_path_points;
    ldtk::IntPoint m_next_point;
    int m_path_step_counter = 0;
    float m_look_around_counter = 0.0f;
    float m_idle_time_counter = 2.0f;
    int breath_attack_frame;

    void CheckAgroSensor();
    void SetAttacking();

    // Inherited via Enemy
    virtual void Update(float dt) override;

    virtual void InitAnimations() override;

    virtual void Die(int death_option) override;

    virtual void TakeDmg(int dmg) override;

    virtual void UpdateIdleState(float dt);

    virtual void UpdateRunningState(float dt);

    virtual void UpdateAttackingState(float dt);

    virtual void UpdateHurtingState(float dt);

    virtual void UpdateDyingState(float dt);

    void UpdatePatrollingState(float dt);

};

