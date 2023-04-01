#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"

class Player : public Entity
{
private:
    Texture2D sprite;
    Animation* m_animation;
    Animations* m_animations;

public:
    Player();
    ~Player();
    
    void update(float dt) override;
    void draw() override;

};
