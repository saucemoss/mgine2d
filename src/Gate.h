#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"
#include "Shard.h"
#include "EnergyOrb.h"



class Gate : public Entity, public Collidable, public Animated, public EnergyEmitter
{
public:
	Gate(const Rectangle& rect);
	~Gate();
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;
	int dmg = 1;
	Shards* gate_shards;
	// Inherited via Animated
	virtual void InitAnimations() override;
	void TakeDmg(int i);

};