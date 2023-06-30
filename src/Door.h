#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Settings.h"
#include "Animations.h"

enum class DoorState
{
    Open,
    Closed,
    Opening,
    Closing
};


class Door : public Collidable, public Entity, public Animated
{
public:
    Door(const Rectangle& rectangle, bool is_right);

    ~Door();
    DoorState state;
    bool m_is_right = true;
    bool m_is_active = true;
    bool player_in_sensor = false;
    b2Fixture* sensor;

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw() override;

    // Inherited via Animated
    virtual void InitAnimations() override;

};