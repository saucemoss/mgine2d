#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"

class FireAxe : public Entity, public Collidable, public Animated
{
public:
	FireAxe(const Rectangle& rect);
	~FireAxe();
	float axvel;

	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};