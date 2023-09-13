#pragma once
#include <iostream>
#include <fstream>
#include "Enemy.h"

enum class FrogBossState
{
	Roar,
	Run,
	Jump,
	AttackCloseAnticipation,
	AttackClose,
	SwirlAttackAnticipation,
	SwirlAttack,
	HookAnticipation,
	RangedHook,
	GroundPound,
	SpawnBalls,
	Dying,
	Decide,
};

class FrogBoss : public Enemy
{
public:
	FrogBoss(const Rectangle& rectangle);
	~FrogBoss();

	FrogBossState boss_state;
	// Inherited via Enemy
	virtual void Update(float dt) override;
	virtual void InitAnimations() override;
	virtual void Die(int death_option) override;
	virtual void TakeDmg(int dmg) override;


	// boss states methods
	void UpdateRoar(float dt);
	void UpdateRun(float dt);
	void UpdateJump(float dt);
	void UpdateAnticipationCloseAttack(float dt);
	void UpdateAttack(float dt);
	void UpdateHook(float dt);
	void UpdateAnticipationHookAttack(float dt);
	void UpdateSwirlAttackAnticipation(float dt);
	void UpdateSwirlAttack(float dt);
	void UpdatePound(float dt);
	void UpdateBalls(float dt);
	void UpdateDie(float dt);
	void UpdateDecide(float dt);

	//state flags/variables
	float attack_counter;
	float random_timer;
	float roar_timer;
	bool player_close = false;
	bool player_mid = false;
	bool player_far = false;
	bool jumped = false;
	bool player_attacked = false;
	bool side_looking_at_att_start = false;
	bool changed_sides;
	bool pound;
	int wall_contacts;
	int prev_states_num[2];
	Vector2 player_pos;

	//	other methods
	void SelectAction();
	void RerollStatesNums(int& rand);
	void SetRun();
	void RunSensor();
	void LookingRight(Vector2 player_pos);
	void DmgPlayer();

	//sensors
	b2Fixture* run_att_sensor;
	b2Fixture* close_att_sensor;
	b2Fixture* ground_pound_sensor;
	b2Fixture* hook_sensor;
	b2Fixture* standing_sensor;
	b2Fixture* running_sensor;
	b2Fixture* swirl_att_sensor;


	// Inherited via Enemy
	virtual void UpdateIdleState(float dt) override;
	virtual void UpdateRunningState(float dt) override;
	virtual void UpdateAttackingState(float dt) override;
	virtual void UpdateHurtingState(float dt) override;
	virtual void UpdateDyingState(float dt) override;

	//Debug
	std::map<FrogBossState, std::string> StatesStrMap{};
};