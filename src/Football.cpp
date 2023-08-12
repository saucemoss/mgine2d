#include "Football.h"
#include "SoundManager.h"
#include "GameScreen.h"

Football::Football(const Rectangle& rectangle) : Enemy({ rectangle.x, rectangle.y, 10, 20 }, FOOTB)
{

	sprite_offset_32 = { -10,-11 };

	InitAnimations();
	m_max_hp = 20;
	m_current_hp = m_max_hp;
	//Physics body cfg
	//add more mass 

	FixtureUserData* data = new FixtureUserData;
	data->tag = ENEMY_GROUP;
	m_fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(data);
	m_fixture->SetDensity(5000.0f);
	m_body->ResetMassData();

	//attack sensor
	m_attack_sensor = util::SimpleSensor(m_body, "footb_att", 0.8f, 0.75f, 0.0f, -0.75f);


}

Football::~Football()
{
	EnitityManager::Remove(this);
}

void Football::Update(float dt)
{

	SwitchFrames(dt);

	switch (state)
	{
	case EnemyState::Idle:
		UpdateIdleState(dt);
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

	}

}

void Football::InitAnimations()
{
	animations->InitializeFootballAnimations();
	SetAnimation("FOOTB_IDLE");
}

void Football::Die(int death_option)
{

	PlayFromFrame(2,"FOOTB_DEAD");
	PlaySound(SoundManager::sounds["hurt12"]);
	PlaySound(SoundManager::sounds["slime2short"]);
	state = EnemyState::Dying;
}

void Football::TakeDmg(int dmg)
{
	if (state != EnemyState::Dying)
	{
		std::string dmgs[] = { "hit2","hit3","hit4" };
		SoundManager::PlayRandSounds(dmgs, 3);
		m_current_hp -= dmg;
		state = EnemyState::Hurting;
		SetAnimation("FOOTB_DMG");
		bleed_particles();
	}
}

void Football::UpdateIdleState(float dt)
{
	if (player_in_dmg_zone)
	{
		PlayFromFrame(2,"FOOTB_ATT");
		state = EnemyState::Attacking;
	}

}


void Football::UpdateAttackingState(float dt)
{
	if (!player_in_dmg_zone && AnimationEnded())
	{
		SetAnimation("FOOTB_IDLE");
		state = EnemyState::Idle;
	}
	else if (animation->GetCurrentFrameNum() >= 8 && animation->GetCurrentFrameNum() <= 10 &&
		player_in_dmg_zone)
	{
		PlaySound(SoundManager::sounds["splat4"]);
		GameScreen::player->take_dmg(10);
	}
	else if (animation->GetCurrentFrameNum() >= 10 &&
		player_in_dmg_zone)
	{
		PlayFromFrame(6, "FOOTB_ATT");
	}
}

void Football::UpdateHurtingState(float dt)
{
	if (AnimationEnded())
	{
		if (m_current_hp <= 0)
		{
			Die(1);
		}
		else
		{
			SetAnimation("FOOTB_IDLE");
			state = EnemyState::Idle;
		}

	}
}

void Football::UpdateDyingState(float dt)
{
	m_body->SetEnabled(false);
	if (AnimationEnded())
	{
		m_destroy = true;
		//if (!LevelManager::world->IsLocked())
		//	LevelManager::world->DestroyBody(m_body);
	}
}

void Football::UpdateRunningState(float dt)
{
}


