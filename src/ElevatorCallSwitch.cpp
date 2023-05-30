#include "ElevatorCallSwitch.h"
#include "LevelManager.h"
#include "GameScreen.h"

ElevatorCallSwitch::ElevatorCallSwitch(const Rectangle& rect, const ldtk::IID elevator_reference)
	:
	m_elevator_reference(elevator_reference)
{

	InitAnimations();
	rectangle = rect;
	x = rectangle.x;
	y = rectangle.y;
	w = rectangle.width;
	h = rectangle.height;

	DisableCollider();
	sensor = { x,y,w,h};
	m_colliderTag = ELEVATOR_CALL_SW;
	state = ECallSwitchState::IDLE;
	EnitityManager::Add(this);
	CollisionManager::Add(this);

}

ElevatorCallSwitch::~ElevatorCallSwitch()
{
	EnitityManager::Remove(this);
	CollisionManager::Remove(this);
}

void ElevatorCallSwitch::Update(float dt)
{
	SwitchFrames(dt);
	player_in_sensor = CollisionManager::IsCollisionWith(PLAYER, sensor);

	switch (state)
	{
	case ECallSwitchState::IDLE:

		if (player_in_sensor && IsKeyPressed(KEY_E) )
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
			elevator->MoveToSwitch(y);
		}
	}
}

void ElevatorCallSwitch::Draw()
{
	auto spritePosX = x;
	auto spritePosY = y;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,settings::drawSize,settings::drawSize },
		{ 0,0 },
		0.0f,
		WHITE);
}

void ElevatorCallSwitch::DrawCollider()
{
	DrawRectangleLinesEx(rectangle, 1, BLUE);
	DrawRectangleLinesEx(sensor, 1, YELLOW);
}

void ElevatorCallSwitch::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeElevatorAnimations();
	SetAnimation("ELEV_SW_IDLE");
}


