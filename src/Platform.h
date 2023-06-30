#pragma once
#include "Entity.h"
#include "Collidable.h"

class Platform : public Collidable, public Entity
{
public:
    Platform();
    Platform(const Rectangle& rectangle);
    ~Platform();

    // Inherited via Entity
    virtual void Draw() override;
    virtual void Update(float dt) override;
};