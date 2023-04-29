#pragma once
#include "Entity.h"
#include "Collisions.h"


class SolidTile : public Collidable
{
public:
    SolidTile();
    SolidTile(const Rectangle& rectangle);

    ~SolidTile();
    void DrawCollider() override;

};