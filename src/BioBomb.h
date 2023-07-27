#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"

class BioBomb : public Entity, public Collidable, public Animated
{
public:
	BioBomb(const Rectangle& rect);
	~BioBomb();
	
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};