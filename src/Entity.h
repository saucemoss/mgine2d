#pragma once
#include <raylib.h>
class Entity
{
public:
    virtual ~Entity() = default;
    virtual void draw() = 0;
    virtual void update(float dt) = 0;
    Vector2 GetPos() const
    {
        return pos;
    }
    Vector2 pos;
};
