#pragma once

#include "Entity.h"
#include <raylib.h>
#include "Animations.h"

class ZSpawner : public Entity, public Animated
{

public:
    ZSpawner();
    ~ZSpawner();

    void update(float dt) override;
    void draw() override;
    void InitAnimations() override;
};