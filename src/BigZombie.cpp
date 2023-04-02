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
	std::cout << "Created Big Zombie" << std::endl;
 }

BigZombie::~BigZombie()
{
	EnitityManager::Remove(this);
}

void BigZombie::update(float dt)
{
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
	if (IsKeyDown(KEY_E))
	{
		EnitityManager::Remove(this);
	}
	
}

void BigZombie::draw()
{
	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ 0,0,64,64 },
		GetPos(),
		0.0f,
		WHITE);
}


void BigZombie::InitAnimations()
{
	sprite = TextureLoader::GetTexture("BIG_Z");
	animations->InitializeBigZAnimations();
	SetAnimation("BIG_ZOMBIE_IDLE");
	
}
