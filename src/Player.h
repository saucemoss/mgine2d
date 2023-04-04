#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"

class Player : public Entity, public Animated
{
public:
    Player();
    ~Player();
    
    void Update(float dt) override;
    void Draw() override;
    void InitAnimations() override;

};
