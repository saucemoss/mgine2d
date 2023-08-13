#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Settings.h"
#include "Animations.h"

enum class TerminalState
{
    Idle,
    Pass,
};


class Terminal : public Collidable, public Entity, public Animated
{
public:
    Terminal(const Rectangle& rectangle);

    ~Terminal();
    TerminalState state;
    bool player_in_sensor;


    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;

    // Inherited via Animated
    virtual void InitAnimations() override;

};