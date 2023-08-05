#include "Platform.h"
#include "GameScreen.h"

Platform::Platform(const Rectangle& rectangle)
	: Collidable(rectangle, b2_staticBody, SOLID)
{
	EnitityManager::Add(this);
}

Platform::~Platform()
{
	EnitityManager::Remove(this);
}

void Platform::Draw(int l)
{
}

void Platform::Update(float dt)
{
	m_fixture->SetSensor(false);

	if (GameScreen::player->pos().y > pos().y)
	{
		m_fixture->SetSensor(true);
	}


	if (IsKeyDown(KEY_DOWN) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > 0.5f)
	{
		m_fixture->SetSensor(true);
	}

}
