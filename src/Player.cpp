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


void Player::Update(float dt)
{
	SwitchFrames(dt);
	SetAnimation("BIGZ_IDLE");
	if (IsKeyDown(KEY_LEFT))
	{
		pos.x += 1;
		SetAnimation("BIGZ_LEFT");

	}
	if (IsKeyDown(KEY_RIGHT))
	{
		pos.x -= 1;
		SetAnimation("BIGZ_RIGHT");

	}
	if (IsKeyDown(KEY_UP))
	{
		pos.y += 1;
		SetAnimation("BIGZ_UP");

	}
	if (IsKeyDown(KEY_DOWN))
	{
		pos.y -= 1;
		SetAnimation("BIGZ_DOWN");

	}
	
}

void Player::Draw()
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
	SetAnimation("BIGZ_IDLE");
}


