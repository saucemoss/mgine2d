#include "BigZombie.h"
#include "Settings.h"
#include "Animations.h"
#include "EntityManager.h"
#include <iostream>

BigZombie::BigZombie()
{
	pos = { -50,-50 };
	m_animations = new Animations();
	m_animations->InitializeBigZAnimations();
	m_animation = m_animations->GetAnimation("BIG_ZOMBIE_IDLE");
	sprite = m_animations->GetTexture();

	EnitityManager::Add(this);
	std::cout << "Created Big Zombie" << std::endl;
 }

BigZombie::~BigZombie()
{
	EnitityManager::Remove(this);
	UnloadTexture(sprite);
	delete m_animations;
}

void BigZombie::update(float dt)
{
	m_animation->SwitchFrames(dt);
	m_animation = m_animations->GetAnimation("BIG_ZOMBIE_IDLE");
	if (IsKeyDown(KEY_LEFT))
	{
		pos.x += 1;
		m_animation = m_animations->GetAnimation("BIG_ZOMBIE_LEFT");

	}
	if (IsKeyDown(KEY_RIGHT))
	{
		pos.x -= 1;
		m_animation = m_animations->GetAnimation("BIG_ZOMBIE_RIGHT");

	}
	if (IsKeyDown(KEY_UP))
	{
		pos.y += 1;
		m_animation = m_animations->GetAnimation("BIG_ZOMBIE_UP");

	}
	if (IsKeyDown(KEY_DOWN))
	{
		pos.y -= 1;
		m_animation = m_animations->GetAnimation("BIG_ZOMBIE_DOWN");
	}
	if (IsKeyDown(KEY_E))
	{
		EnitityManager::Remove(this);
	}
	
}

void BigZombie::draw()
{
	DrawTexturePro(sprite,
		m_animation->GetCurrentFrame(),
		Rectangle{ 0,0,64,64 },
		pos,
		0.0f,
		WHITE);
}

void BigZombie::SetPos(Vector2 pos_in)
{
	pos = pos_in;
}
