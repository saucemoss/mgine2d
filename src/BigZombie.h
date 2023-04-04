#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"

class BigZombie : public Entity, public Animated
{

public:
    BigZombie();
    ~BigZombie();

    void Update(float dt) override;
    void Draw() override;
    void InitAnimations() override;

};