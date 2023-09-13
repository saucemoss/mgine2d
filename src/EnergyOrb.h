#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"
#include "GameScreen.h"

class EnergyOrb : public Entity, public Collidable, public Animated
{
public:
	EnergyOrb(const Rectangle& rect);
	~EnergyOrb();

	Vector2 target;
	Vector2 random_spread_impulse;
	bool axed = false;
	float time_to_live = 5.0f;
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};

class EnergyEmitter
{
public:
	virtual void SpawnOrbs(int how_many, Vector2 pos)
	{
		for (int i = 0; i < how_many; i++)
		{
			GameScreen::LevelMgr->queue_entities.push_back({ "EOrb", pos });
		}
		
	}
};