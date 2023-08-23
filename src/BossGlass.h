#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Particles.h"

enum class BossGlassState
{
    Idle,
    Knocking,
    Shattered
    
};

class BossGlass : public Collidable, public Entity, public Animated
{
public:
    BossGlass(const Rectangle& rectangle);

    ~BossGlass();
    ParticleEmitter* p_emitter1;
    ParticleEmitter* p_emitter2;

    BossGlassState state;

    bool player_entered_sensor;
    bool boss_added = false;
    bool level_unlocked = true;
    float delay_timer;

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;

    // Inherited via Animated
    virtual void InitAnimations() override;

};