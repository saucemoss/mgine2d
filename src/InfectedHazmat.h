#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collidable.h"
#include <map>
#include <box2d.h>
#include "Enemy.h"



class InfectedHazmat : public Enemy
{
public:
    InfectedHazmat(const Rectangle& rectangle);
    ~InfectedHazmat();

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

};

