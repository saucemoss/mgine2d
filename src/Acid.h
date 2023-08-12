#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Particles.h"


class Acid : public Collidable, public Entity
{
public:
    Acid(const Rectangle& rectangle);

    ~Acid();
    ParticleEmitter* p_emitter1;
    ParticleEmitter* p_emitter2;

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;


};