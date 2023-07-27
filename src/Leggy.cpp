#include "Leggy.h"
#include "SoundManager.h"
#include "GameScreen.h"

Leggy::Leggy(const Rectangle& rectangle) : Enemy({ rectangle.x, rectangle.y, 14, 30 }, LEGGY)
{

	InitAnimations();
	m_max_hp = 100;
	m_current_hp = m_max_hp;
	//Physics body cfg
	//add more mass 

	FixtureUserData* data = new FixtureUserData;
	data->tag = ENEMY_GROUP;
	m_fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(data);
	m_fixture->SetDensity(50.0f);
	m_body->ResetMassData();


	//feet sensor
	m_feet_sensor = util::SimpleSensor(m_body, "leg_feet", 0.3f, 0.3f, 0, 0.80f);
	//player agro sensor
	util::SimpleSensor(m_body, "leg_agro", 4.0f, 2.0f, -3.0, 0.0);
	util::SimpleSensor(m_body, "leg_agro", -4.0f, 2.0f, 3.0, 0.0);
	//attack sensor
	m_attack_sensor = util::SimpleSensor(m_body, "leg_att", 3.0f, 0.61f);

	m_body->SetLinearDamping(linear_dumping);

}

Leggy::~Leggy()
{
	EnitityManager::Remove(this);
}

void Leggy::Update(float dt)
{

	SwitchFrames(dt);
	CheckTouchGround();

	if (player_in_agro && GameScreen::player->center_pos().x > center_pos().x)
	{
		looking_right = true;
	}
	else if (player_in_agro && GameScreen::player->center_pos().x < center_pos().x)
	{
		looking_right = false;
	}

	switch (state)
	{
	case EnemyState::Idle:
		UpdateIdleState(dt);
		break;
	case EnemyState::Running:
		UpdateRunningState(dt);
		break;
	case EnemyState::Attacking:
		UpdateAttackingState(dt);
		break;
	case EnemyState::Hurting:
		UpdateHurtingState(dt);
		break;
	case EnemyState::Dying:
		UpdateDyingState(dt);
		break;
	case EnemyState::Spawning:
		UpdateSpawningState(dt);
		break;
	}

}

void Leggy::InitAnimations()
{
	animations->InitializeLeggyAnimations();
	SetAnimation("LEGGY_IDLE");
}

void Leggy::Die(int death_option)
{

	PlayFromFrame(2, "LEGGY_DEAD");
	PlaySound(SoundManager::sounds["hurt12"]);
	PlaySound(SoundManager::sounds["slime2short"]);
	state = EnemyState::Dying;
}

void Leggy::TakeDmg(int dmg)
{
	if (state != EnemyState::Dying)
	{
		std::string dmgs[] = { "hit2","hit3","hit4" };
		SoundManager::PlayRandSounds(dmgs, 3);
		m_current_hp -= dmg;
		state = EnemyState::Hurting;
		SetAnimation("LEGGY_DMG");
	}
}

void Leggy::UpdateIdleState(float dt)
{
	if (player_in_agro)
	{
		state = EnemyState::Running;
		SetAnimation("LEGGY_RUN");
	}
}

void Leggy::UpdateRunningState(float dt)
{
	if (!player_in_agro)
	{
		state = EnemyState::Idle;
		SetAnimation("LEGGY_IDLE");
	}
	else
	{
		if (looking_right)
		{
			set_velocity_x(speed);
		}
		else
		{
			set_velocity_x(-speed);
		}
	}

	if (player_in_dmg_zone)
	{
		SetAnimation("LEGGY_ATT");
		state = EnemyState::Attacking;
	}


	if (player_in_dmg_zone)
	{
		SetAnimation("LEGGY_ATT");
		state = EnemyState::Attacking;
	}

}

void Leggy::UpdateAttackingState(float dt)
{
	if (!player_in_dmg_zone)
	{
		SetAnimation("LEGGY_IDLE");
		state = EnemyState::Idle;
	}
	if (animation->GetCurrentFrameNum() >= 4 &&
		player_in_dmg_zone)
	{
		PlaySound(SoundManager::sounds["splat4"]);
		GameScreen::player->take_dmg(10);
	}
}

void Leggy::UpdateHurtingState(float dt)
{
	if (AnimationEnded())
	{
		if (m_current_hp <= 0)
		{
			Die(1);
		}
		else
		{
			SetAnimation("LEGGY_IDLE");
			state = EnemyState::Idle;
		}

	}
}

void Leggy::UpdateDyingState(float dt)
{
	if (is_touching_floor)
	{
		m_body->SetEnabled(false);

	}
	if (AnimationEnded())
	{
		m_destroy = true;
		if (!LevelManager::world->IsLocked())
			LevelManager::world->DestroyBody(m_body);
	}
}

void Leggy::UpdateSpawningState(float dt)
{
	if (AnimationEnded())
	{
		SetAnimation("LEGGY_IDLE");
		state = EnemyState::Idle;
	}
}

