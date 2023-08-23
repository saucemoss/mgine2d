#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "Entity.h"
#include "Collidable.h"
#include "Settings.h"
#include "Animations.h"

enum class MediPodState
{
    Open,
    Closed,
    Opening,
    Closing,
    Locked,
    Saving,
};


class MediPod : public Collidable, public Entity, public Animated
{
public:
    MediPod(const Rectangle& rectangle);

    ~MediPod();
    MediPodState state;

    bool player_in_sensor = false;
    bool saved = false;
    b2Fixture* sensor;

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;

    // Inherited via Animated
    virtual void InitAnimations() override;

    void SaveGame();

};