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
};


class Elevator : public Collidable, public Entity, public Animated
{
public:
    Elevator(const Rectangle& rectangle, const ldtk::ArrayField<int> levels, const ldtk::Entity& ldtk_elevator);

    ~Elevator();
    ElevatorState state;
    Rectangle sensor;
    Rectangle roof;
    const ldtk::Entity& m_ldtk_elevator;
    void ResetY(int next_level);

    void MoveUp(int speed);

    void MoveDown(int speed);

    void MoveToSwitch(int y_in);

    // Inherited via Entity
    virtual void Update(float dt) override;

    // Inherited via Entity
    virtual void Draw() override;


    // Inherited via Collidable
    virtual void DrawCollider() override;


    // Inherited via Animated
    virtual void InitAnimations() override;
private:
    bool m_is_active = true;
    bool player_in_sensor = false;
    bool is_reset;
    bool m_going_up = false;
    int m_distance_counter = 0;
    int m_current_level;
    int m_speed = 3;
    int next_level;
    ldtk::ArrayField<int> m_levels;


};