#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"
#include "NPCSecurityGuy.h"


class NPCSecurityGuy2 : public Entity, public Collidable, public Animated
{
public:
	NPCSecurityGuy2(const Rectangle& rect);
	~NPCSecurityGuy2();
	bool looking_right = true;
	bool player_in_sensor = false;
	bool enemy_in_sensor = false;
	bool dialogue1_exhausted = false;
	NPCState state;
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};