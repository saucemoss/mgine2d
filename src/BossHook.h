#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"

class BossHook : public Entity, public Collidable, public Animated
{
public:
	BossHook(const Rectangle& rect, bool going_right);
	~BossHook();

	Vector2 target;
	bool going_right = true;
	bool axed = false;
	bool player_hit = false;
	float time_to_live;
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;

	// Inherited via Animated
	virtual void InitAnimations() override;
};