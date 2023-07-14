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
    // Inherited via Entity
    virtual void Update(float dt) override;
    bool ElevatorAtSwitch();

    void CallElevator();

    // Inherited via Entity
    virtual void Draw(int l) override;

    // Inherited via Animated
    virtual void InitAnimations() override;

    bool player_in_sensor = false;

private:

   
    const ldtk::IID m_elevator_reference;
    float press_delay;
    float press_delay_counter;
    

};