#include "Player.h"
#include "EntityManager.h"
#include "Settings.h"
#include "Animations.h"
#include <iostream>

Player::Player()
{
	SetPos({ 0,0 });
	InitAnimations();
	EnitityManager::Add(this);
}

Player::~Player()
{
	EnitityManager::Remove(this);
}


void Player::update(float dt)
{
	SwitchFrames(dt);
	SetAnimation("BIG_ZOMBIE_IDLE");
	if (IsKeyDown(KEY_LEFT))
	{
		pos.x += 1;
		SetAnimation("BIG_ZOMBIE_LEFT");
		
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		pos.x -= 1;
		SetAnimation("BIG_ZOMBIE_RIGHT");

	}
	if (IsKeyDown(KEY_UP))
	{
		pos.y += 1;
		SetAnimation("BIG_ZOMBIE_UP");

	}
	if (IsKeyDown(KEY_DOWN))
	{
		pos.y -= 1;
		SetAnimation("BIG_ZOMBIE_DOWN");

	}
	
}

void Player::draw()
{
	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{0,0,64,64},
		GetPos(),
		0.0f,
		WHITE);
}

void Player::InitAnimations()
{
	sprite = TextureLoader::GetTexture("BIG_Z");
	animations->InitializeBigZAnimations();
	SetAnimation("BIG_ZOMBIE_IDLE");
}


