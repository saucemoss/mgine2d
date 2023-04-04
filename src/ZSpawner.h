#pragma once

#include "Entity.h"
#include <raylib.h>
#include "Animations.h"

class ZSpawner : public Entity, public Animated
{

public:
    ZSpawner();
    ~ZSpawner();

    void Update(float dt) override;
    void Draw() override;
    void InitAnimations() override;
};