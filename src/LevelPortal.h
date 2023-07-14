#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Settings.h"
#include "LDtkLoader/Entity.hpp"

class LevelPortal : public Collidable, public Entity
{
public:
    LevelPortal(const Rectangle& rectangle, const std::string to_level, const ldtk::IID iid_reference);

    ~LevelPortal();

    const std::string m_to_level;
    //const std::string m_level;
    const ldtk::IID m_iid_reference;
    bool is_active = true;

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;

};