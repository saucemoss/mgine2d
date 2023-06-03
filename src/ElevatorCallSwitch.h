#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Settings.h"
#include "Animations.h"
#include "LDtkLoader/Project.hpp"
#include "Elevator.h"
enum class ECallSwitchState
{
    IDLE,
    PRESSED
};
class ElevatorCallSwitch : public Collidable, public Entity, public Animated
{
public:
    ElevatorCallSwitch(const Rectangle& rectangle, const ldtk::IID elevator_reference);

    ~ElevatorCallSwitch();
    ECallSwitchState state;
    Rectangle sensor;
    // Inherited via Entity
    virtual void Update(float dt) override;

    bool ElevatorAtSwitch();

    void CallElevator();

    // Inherited via Entity
    virtual void Draw() override;


    // Inherited via Collidable
    virtual void DrawCollider() override;


    // Inherited via Animated
    virtual void InitAnimations() override;



private:

    bool player_in_sensor = false;
    const ldtk::IID m_elevator_reference;
    float press_delay;
    float press_delay_counter;
    

};