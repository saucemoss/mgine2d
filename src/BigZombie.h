#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"

class BigZombie : public Entity, public Animated
{

public:
    BigZombie();
    ~BigZombie();

    void update(float dt) override;
    void draw() override;
    void InitAnimations() override;

};