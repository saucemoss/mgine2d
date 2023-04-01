#include "Player.h"
#include "EntityManager.h"
#include "Settings.h"
#include "Animations.h"
#include <iostream>

Player::Player()
{
	pos = { 0,0 };
	m_animations = new Animations();
	m_animations->InitializeBigZAnimations();
	m_animation = m_animations->GetAnimation("BIG_ZOMBIE_IDLE");
	sprite = m_animations->GetTexture();
	EnitityManager::Add(this);
}

Player::~Player()
{
	UnloadTexture(sprite);
	delete m_animations;
}


void Player::update(float dt)
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
	
}

void Player::draw()
{
	DrawTexturePro(sprite,
		m_animation->GetCurrentFrame(),
		Rectangle{0,0,64,64},
		pos,
		0.0f,
		WHITE);
}


