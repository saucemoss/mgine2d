#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "Entity.h"
#include "Collidable.h"
#include "Settings.h"
#include "Animations.h"


enum class SwitchState
{
    Off,
    On,
};


class Switch : public Collidable, public Entity, public Animated
{
public:
    Switch(const Rectangle& rectangle);

    ~Switch();
    SwitchState state;
    bool axed;
    ldtk::IID m_elevator_reference;
    bool saved = false;

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;

    // Inherited via Animated
    virtual void InitAnimations() override;

};