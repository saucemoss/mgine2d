#pragma once
#include <raylib.h>
class Entity
{
public:
    virtual ~Entity() = default;
    virtual void Draw() = 0;
    virtual void Update(float dt) = 0;
    Vector2 GetPos()    {
        return pos;
    }
    void SetPos(Vector2 pos_in)
    {
        pos = pos_in;
    }
    void SetPos(float x, float y)
    {
        pos = { x,y };
    }
protected:
    Vector2 pos;
};
