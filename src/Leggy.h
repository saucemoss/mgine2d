#pragma once
#include "Enemy.h"

class Leggy : public Enemy
{
public:
	Leggy(const Rectangle& rectangle);
	~Leggy();


	// Inherited via Enemy
	virtual void Update(float dt) override;
	virtual void InitAnimations() override;
	virtual void Die(int death_option) override;
	virtual void TakeDmg(int dmg) override;
	virtual void UpdateIdleState(float dt);
	virtual void UpdateRunningState(float dt);
	virtual void UpdateAttackingState(float dt);
	virtual void UpdateHurtingState(float dt);
	virtual void UpdateDyingState(float dt);
	void UpdateSpawningState(float dt);

};