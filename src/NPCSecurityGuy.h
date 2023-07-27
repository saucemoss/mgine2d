#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"

class NPCSecurityGuy : public Entity, public Collidable, public Animated
{
public:
	NPCSecurityGuy(const Rectangle& rect);
	~NPCSecurityGuy();
	bool looking_right = true;
	bool player_in_sensor = false;
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};