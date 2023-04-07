#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include <box2d.h>

class Player : public Entity, public Animated
{
public:
    Player(b2World* physicsWorld);
    ~Player();
    
    void Update(float dt) override;
    void Draw() override;
    void InitAnimations() override;
   
    b2Body* body{};
    b2World* physicsWorld;
};
