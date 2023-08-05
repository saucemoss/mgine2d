#pragma once
#include "Collidable.h"


class SolidTile : public Collidable
{
public:
    SolidTile();
    SolidTile(const Rectangle& rectangle);
    SolidTile(const Rectangle& rectangle, bool is_floor);
    SolidTile(const b2Vec2* verices, const int vertices_count, const Rectangle& rectangle);
    ~SolidTile();
    bool is_slope = false;
    bool is_floor = false;
};