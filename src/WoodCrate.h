#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"
#include "Shard.h"



class WoodCrate : public Entity, public Collidable, public Animated
{
public:
	WoodCrate(const Rectangle& rect);
	~WoodCrate();
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;
	int dmg = 1;
	Shards* crate_shards;
	// Inherited via Animated
	virtual void InitAnimations() override;
	void TakeDmg(int i);

};