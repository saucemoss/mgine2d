#pragma once
#include "Collidable.h"


class SolidTile : public Collidable
{
public:
    SolidTile();
    SolidTile(const Rectangle& rectangle);
    ~SolidTile();
};