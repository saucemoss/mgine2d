#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"

class Player : public Entity, public Animated
{
public:
    Player();
    ~Player();
    
    void update(float dt) override;
    void draw() override;
    void InitAnimations() override;

};
