#pragma once
#include "Entity.h"
#include <box2d.h>

class Ground : public Entity
{
public:
    Ground(b2World* physicsWorld);
    ~Ground();

    void Update(float dt) override;
    void Draw() override;


    b2Body* body{};
    b2World* physicsWorld;
};