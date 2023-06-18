#pragma once
#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"



class WoodCrate : public Entity, public Collidable, public Animated
{
public:
	WoodCrate(const Rectangle& rect);
	~WoodCrate();
	// Inherited via Entity
	virtual void Draw() override;
	virtual void Update(float dt) override;


	// Inherited via Animated
	virtual void InitAnimations() override;

};