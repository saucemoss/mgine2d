#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"

class BioBreath : public Entity, public Collidable, public Animated
{
public:
	BioBreath(const Rectangle& rect, Vector2 direction);
	~BioBreath();
	Vector2 direction;
	bool axed = false;
	float time_to_live;
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};