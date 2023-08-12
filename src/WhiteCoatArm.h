#pragma once
#include "Enemy.h"


class WhiteCoatArm : public Enemy
{
public:
	// Inherited via Enemy
	virtual void Update(float dt) override;
	virtual void InitAnimations() override;
	virtual void Die(int death_option) override;
	void CheckAgroSensor();
	virtual void TakeDmg(int dmg) override;
	virtual void UpdateIdleState(float dt) override;
	virtual void UpdateRunningState(float dt) override;
	void SetAttacking();
	virtual void UpdateAttackingState(float dt) override;
	virtual void UpdateHurtingState(float dt) override;
	virtual void UpdateDyingState(float dt) override;
	WhiteCoatArm(const Rectangle& rectangle);
	~WhiteCoatArm();



};
