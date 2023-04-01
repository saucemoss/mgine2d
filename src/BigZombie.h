#pragma once
#include "Entity.h"
#include <raylib.h>
#include "Animations.h"

class BigZombie : public Entity
{
private:
    Texture2D sprite;
    Animation* m_animation;
    Animations* m_animations;

public:
    BigZombie();
    ~BigZombie();

    void update(float dt) override;
    void draw() override;
    void SetPos(Vector2 pos_in);

};