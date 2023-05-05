#pragma once
#include "Entity.h"
#include "Collisions.h"
#include "EntityManager.h"
#include "Settings.h"

class LevelPortal : public Collidable, public Entity
{
public:
    LevelPortal();
    LevelPortal(const Rectangle& rectangle, std::string to_level, float xNewPlayerPos, float yNewPlayerPos);

    ~LevelPortal();
    void Draw() override;
    void DrawCollider() override;
    std::string m_to_level;
    float m_xNewPlayerPos;
    float m_yNewPlayerPos;


    // Inherited via Entity
    virtual void Update(float dt) override;

};