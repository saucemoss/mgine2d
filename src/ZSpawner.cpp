#include "ZSpawner.h"
#include "Settings.h"
#include "Animations.h"
#include "EntityManager.h"
#include <iostream>

ZSpawner::ZSpawner()
{
	SetPos({ -150,-150 });
	std::cout << GetPos().x << std::endl;
	InitAnimations();
	EnitityManager::Add(this);
}

ZSpawner::~ZSpawner()
{
	EnitityManager::Remove(this);
}

void ZSpawner::Update(float dt)
{
	SwitchFrames(dt);
	if (AnimationEnded()) {
		SetAnimation("ZSPAWNER_IDLE");
	}
	if (IsKeyPressed(KEY_E))
	{
		PlayOnce("ZSPAWNER_EMERGE");
	}

	
}

void ZSpawner::Draw()
{
	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ 0,0,64,64 },
		GetPos(),
		0.0f,
		WHITE);
}

void ZSpawner::InitAnimations()
{
	sprite = TextureLoader::GetTexture("Z_SPAWNER");
	animations->InitializeZSpawnerAnimations();
	SetAnimation("ZSPAWNER_IDLE");
}
