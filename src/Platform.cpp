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

void Platform::Draw()
{
}

void Platform::Update(float dt)
{
	m_fixture->SetSensor(false);

	if (GameScreen::player->pos().y > pos().y)
	{
		m_fixture->SetSensor(true);
	}

	if (LevelManager::CheckPlayerInSensor(*m_fixture))
	{
		if (IsKeyPressed(KEY_DOWN))
		{
			m_fixture->SetSensor(true);
		}
	}
}
