#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"
#include "Dialogue.h"

enum class NPCDoc1State
{
	Idle_first_meet, Talk_first_meet, Idle, Talk, Run, Work
};

class NPCDoc1 : public Entity, public Collidable, public Animated
{
public:
	NPCDoc1(const Rectangle& rect);
	~NPCDoc1();
	bool looking_right = true;
	bool player_in_sensor = false;
	bool visible = true;
	NPCDoc1State state;

	b2Fixture* first_meet_sensor;

	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};