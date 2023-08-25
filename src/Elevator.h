#pragma once
#include "Entity.h"
#include "ElevatorCallSwitch.h"
#include "Collidable.h"
#include "Settings.h"
#include "Animations.h"
#include "LDtkLoader/Project.hpp"

enum class ElevatorState
{
    START_LEVEL,
    NEXT_LEVEL,
    PREVIOUS_LEVEL,
    END_LEVEL,
    GOING_DOWN,
    GOING_UP,
    GOING_TO_SW,
    AT_SW,
    OPENING,
    CLOSING,
    UNPOWERED,
};


class Elevator : public Collidable, public Entity, public Animated
{
public:
    Elevator(const Rectangle& rectangle, const ldtk::ArrayField<int> levels, const ldtk::Entity& ldtk_elevator);

    ~Elevator();
    ElevatorState state;
    b2Fixture* walls_fixture;
    Texture2D* bgsprite;
    bool powered = true;

    const ldtk::Entity& m_ldtk_elevator;
    void ResetY(int next_level);

    void MoveUp(float speed);

    void MoveDown(float speed);

    void MoveToSwitch(float y_in);

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw(int l) override;

    // Inherited via Animated
    virtual void InitAnimations() override;
private:
    bool m_is_active = true;
    bool is_reset;
    bool m_going_up = false;
    int m_distance_counter = 0;
    int m_current_level;
    int m_speed = 8;
    int next_level;
    ldtk::ArrayField<int> m_levels;


};