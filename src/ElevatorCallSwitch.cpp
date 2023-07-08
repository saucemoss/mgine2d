#include "ElevatorCallSwitch.h"
#include "LevelManager.h"
#include "GameScreen.h"

ElevatorCallSwitch::ElevatorCallSwitch(const Rectangle& rect, const ldtk::IID elevator_reference)
	:
	Collidable(rect, b2_staticBody, ELEVATOR_CALL_SW),
	m_elevator_reference(elevator_reference)
{
	m_fixture->SetSensor(true);
	InitAnimations();

	state = ECallSwitchState::IDLE;
	EnitityManager::Add(this);
}

ElevatorCallSwitch::~ElevatorCallSwitch()
{
	EnitityManager::Remove(this);
}

void ElevatorCallSwitch::Update(float dt)
{
	SwitchFrames(dt);
	player_in_sensor = LevelManager::CheckPlayerInSensor(*m_fixture);

	switch (state)
	{
	case ECallSwitchState::IDLE:

		if (player_in_sensor && (IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)))
		{
			SetAnimation("ELEV_SW_PRESSED");
			CallElevator();
			state = ECallSwitchState::PRESSED;
		}
		
		break;
	case ECallSwitchState::PRESSED:

		if (ElevatorAtSwitch())
		{
			SetAnimation("ELEV_SW_IDLE");
			state = ECallSwitchState::IDLE;

		}
		break;
	}

}


bool ElevatorCallSwitch::ElevatorAtSwitch()
{
	for (Entity* e : EnitityManager::EntityList)
	{
		if (e->m_ldtkID == m_elevator_reference)
		{
			Elevator* elevator = dynamic_cast<Elevator*>(e);
			if (elevator->state == ElevatorState::AT_SW)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

void ElevatorCallSwitch::CallElevator()
{
	for (Entity* e : EnitityManager::EntityList)
	{
		if (e->m_ldtkID == m_elevator_reference)
		{
			Elevator* elevator = dynamic_cast<Elevator*>(e);
			elevator->MoveToSwitch(m_body->GetPosition().y * settings::PPM);
		}
	}
}

void ElevatorCallSwitch::Draw()
{
	auto spritePosX = center_pos().x;
	auto spritePosY = center_pos().y;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
		{ 0,0 },
		0.0f,
		WHITE);
}


void ElevatorCallSwitch::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeElevatorAnimations();
	SetAnimation("ELEV_SW_IDLE");
}


