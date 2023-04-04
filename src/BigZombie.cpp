#include "BigZombie.h"
#include "Settings.h"
#include "Animations.h"
#include "EntityManager.h"
#include <iostream>

BigZombie::BigZombie()
{
	SetPos({ -50,-50 });
	InitAnimations();
	EnitityManager::Add(this);
 }

BigZombie::~BigZombie()
{
	EnitityManager::Remove(this);
}

void BigZombie::Update(float dt)
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
	if (IsKeyDown(KEY_E))
	{
		EnitityManager::Remove(this);
	}
	
}

void BigZombie::Draw()
{
	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ 0,0,62,62 },
		pos,
		0.0f,
		WHITE);
}


void BigZombie::InitAnimations()
{
	sprite = TextureLoader::GetTexture("BIG_Z");
	animations->InitializeBigZAnimations();
	SetAnimation("BIGZ_IDLE");
	
}
