#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"
#include "Dialogue.h"

enum class NPCDoc1State
{
	Idle_first_meet, Talk_first_meet, Idle, Talk, GoToTerminal,GoToStartButton, Work, Talk_welcome, Talk_idea, Talk_help, Talk_explain, Talk_lore
};

class NPCDoc1 : public Entity, public Collidable, public Animated
{
public:
	NPCDoc1(const Rectangle& rect);
	~NPCDoc1();
	void CheckLookingSide();

	bool looking_right = true;
	bool player_in_sensor = false;
	bool visible = true;
	bool entangeler_loaded = false;
	bool terminals_on = false;
	float speed = 3.0f;
	float work_timer = 3.0f;
	NPCDoc1State state;

	b2Fixture* first_meet_sensor;

	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	void ApplyOutlineOnSensor();


	// Inherited via Animated
	virtual void InitAnimations() override;
};