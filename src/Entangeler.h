#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "Entity.h"
#include "Collidable.h"
#include "Settings.h"
#include "Animations.h"
#include "NPCDoc1.h"

enum class EntangelerState
{
    Idle,
    Loaded,
    Working,
    Finished
};


class Entangeler : public Collidable, public Entity, public Animated
{
public:
    Entangeler(const Rectangle& rectangle);

    ~Entangeler();
    EntangelerState state;
    bool terminals_on = false;
    int terminal_on_num = 0;
    NPCDoc1* doc_ptr;

    std::vector<std::string> terminals;

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;

    // Inherited via Animated
    virtual void InitAnimations() override;

};