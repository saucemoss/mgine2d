#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"
#include "Dialogue.h"

enum NPCState
{
	Idling, TalkingOption1, TalkingOption2, Running
};

class NPCSecurityGuy : public Entity, public Collidable, public Animated
{
public:
	NPCSecurityGuy(const Rectangle& rect);
	~NPCSecurityGuy();
	bool looking_right = true;
	bool player_in_sensor = false;
	bool enemy_in_sensor = false;
	bool dialogue1_exhausted = false;
	bool dialogue7_exhausted = false;
	NPCState state;
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};