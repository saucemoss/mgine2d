#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"

class BioBomb : public Entity, public Collidable, public Animated
{
public:
	BioBomb(const Rectangle& rect);
	~BioBomb();

	Vector2 target;

	bool axed = false;
	float time_to_live;
	Vector2 random_spread_impulse;
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};