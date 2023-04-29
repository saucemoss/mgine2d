#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"
#include "Collisions.h"
#include <box2d.h>

class Player : public Entity, public Animated, public Collidable
{
public:
    Player();
    ~Player();
    
    void Update(float dt) override;
    void Draw() override;
    void DrawCollider() override;
    void InitAnimations() override;

    //Movement Controll
    bool is_touching_floor = false;
    bool looking_right = true;
    Rectangle feetSensor;
};
