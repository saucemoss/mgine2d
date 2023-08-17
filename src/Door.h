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
    Closing,
    Locked,
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
    bool locked = false;
    bool denied_played = false;
    int terminal_unlocks;

    ldtk::ArrayField<ldtk::EntityRef> refs;

    b2Fixture* sensor;
    Sound my_engine_sound;

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;

    // Inherited via Animated
    virtual void InitAnimations() override;

};