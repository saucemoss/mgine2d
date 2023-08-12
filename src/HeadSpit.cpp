#include "HeadSpit.h"
#include "SoundManager.h"
#include "GameScreen.h"
#include "BioBomb.h"

HeadSpit::HeadSpit(const Rectangle& rectangle) : Enemy({ rectangle.x, rectangle.y, 10, 20 }, HSPIT)
{
	sprite_offset_32 = { -10,-12 };
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
	//player agro sensor
	util::SimpleSensor(m_body, "hspit_att", 6.0f, 2.0f, -3.0, 0.0);

}

HeadSpit::~HeadSpit()
{
	EnitityManager::Remove(this);
}

void HeadSpit::Update(float dt)
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

void HeadSpit::InitAnimations()
{
	animations->InitializeHeadSpitAnimations();
	SetAnimation("HSPIT_IDLE");
}

void HeadSpit::Die(int death_option)
{

	PlayFromFrame(2, "HSPIT_DEAD");
	PlaySound(SoundManager::sounds["hurt12"]);
	PlaySound(SoundManager::sounds["slime2short"]);
	state = EnemyState::Dying;
}

void HeadSpit::TakeDmg(int dmg)
{
	if (state != EnemyState::Dying)
	{
		std::string dmgs[] = { "hit2","hit3","hit4" };
		SoundManager::PlayRandSounds(dmgs, 3);
		m_current_hp -= dmg;
		state = EnemyState::Hurting;
		SetAnimation("HSPIT_DMG");
		bleed_particles();
	}
}

void HeadSpit::UpdateIdleState(float dt)
{
	if (player_in_dmg_zone)
	{
		PlayFromFrame(2, "HSPIT_ATT");
		state = EnemyState::Attacking;
	}

}


void HeadSpit::UpdateAttackingState(float dt)
{
	if (!player_in_dmg_zone && AnimationEnded())
	{
		SetAnimation("HSPIT_IDLE");
		state = EnemyState::Idle;
		shot = false;
	}
	else if (animation->GetCurrentFrameNum() >= 6 &&
		player_in_dmg_zone && shot == false)
	{
		shot = true;
		PlaySound(SoundManager::sounds["splat4"]);
		//GameScreen::player->take_dmg(10);
		Rectangle rect = Rectangle{ center_pos().x , center_pos().y - 1.0f , 16, 16 };
		LevelManager::level_entities_safe.push_back(std::make_unique<BioBomb>(rect));
		BioBomb* bomb = reinterpret_cast<BioBomb*>(LevelManager::level_entities_safe.back().get());
		bomb->m_body->ApplyAngularImpulse((looking_right ? 30.5f : -30.5f), true);
		bomb->m_body->ApplyLinearImpulseToCenter({ -100.0f, -100.0f }, true);
	}
}

void HeadSpit::UpdateHurtingState(float dt)
{
	if (AnimationEnded())
	{
		if (m_current_hp <= 0)
		{
			Die(1);
		}
		else
		{
			SetAnimation("HSPIT_IDLE");
			state = EnemyState::Idle;
		}

	}
}

void HeadSpit::UpdateDyingState(float dt)
{
	m_body->SetEnabled(false);
	if (AnimationEnded())
	{
		m_destroy = true;
		//if (!LevelManager::world->IsLocked())
		//	LevelManager::world->DestroyBody(m_body);
	}
}

void HeadSpit::UpdateRunningState(float dt)
{
}


