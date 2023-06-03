#pragma once
#include "Entity.h"
#include "Collidable.h"


class SolidTile : public Collidable
{
public:
    SolidTile();
    SolidTile(const Rectangle& rectangle);

    ~SolidTile();
    
    // Inherited via Collidable
    virtual void DrawCollider() override;

};