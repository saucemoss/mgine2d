#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "Entity.h"
#include "Collidable.h"
#include "Particles.h"


class SecretFog : public Collidable, public Entity
{
public:
    SecretFog(const Rectangle& rectangle);

    ~SecretFog();
    bool revealed = false;
    bool sound_played;
    float alpha_reveal = 1.0f;

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;


};