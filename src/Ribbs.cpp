#include "Ribbs.h"
#include "SoundManager.h"
#include "GameScreen.h"
#include "Leggy.h"

Ribbs::Ribbs(const Rectangle& rectangle) : Enemy({ rectangle.x, rectangle.y, 14, 30 }, RIBBS)
{

	InitAnimations();
	m_max_hp = 220;
	m_current_hp = m_max_hp;
	//Physics body cfg
	//add more mass 

	FixtureUserData* data = new FixtureUserData;
	data->tag = ENEMY_GROUP;
	m_fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(data);
	m_fixture->SetDensity(100.0f);
	m_body->ResetMassData();


	//feet sensor
	m_feet_sensor = util::SimpleSensor(m_body, "rib_feet", 0.3f, 0.3f, 0, 0.80f);
	//player agro sensor
	util::SimpleSensor(m_body, "rib_agro", 2.0f, 2.0f,-3.0, 0.0);
	util::SimpleSensor(m_body, "rib_agro", -2.0f, 2.0f, 3.0, 0.0);
	//attack sensor
	m_attack_sensor = util::SimpleSensor(m_body, "rib_att", 2.0f, 0.61f);

	m_body->SetLinearDamping(linear_dumping);

}

Ribbs::~Ribbs()
{
	EnitityManager::Remove(this);
}

void Ribbs::Update(float dt)
{
	dmg_counter += dt;
	if (dmg_counter >= 1.0f)
	{
		taking_dmg = false;
	}

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
	}

}

void Ribbs::InitAnimations()
{
	animations->InitializeRibbsAnimations();
	SetAnimation("RIBBS_IDLE");
}

void Ribbs::Die(int death_option)
{

	SetAnimation("RIBBS_DEAD");
	PlaySound(SoundManager::sounds["hurt12"]);
	PlaySound(SoundManager::sounds["slime2short"]);
	state = EnemyState::Dying;
}

void Ribbs::TakeDmg(int dmg)
{
	if (state != EnemyState::Dying && dmg_counter >= 1.0f)
	{
		taking_dmg = true;
		dmg_counter = 0.0f;
		std::string dmgs[] = { "hit2","hit3","hit4" };
		SoundManager::PlayRandSounds(dmgs, 3);
		m_current_hp -= dmg;
		state = EnemyState::Hurting;
		SetAnimation("RIBBS_DMG");
		
		if (m_current_hp <= 0)
		{
			Die(1);
		}
	}

}

void Ribbs::UpdateIdleState(float dt)
{
	if (player_in_agro)
	{
		state = EnemyState::Running;
		SetAnimation("RIBBS_RUN");
	}
}

void Ribbs::UpdateRunningState(float dt)
{
	if (!player_in_agro)
	{
		state = EnemyState::Idle;
		SetAnimation("RIBBS_IDLE");
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
		SetAnimation("RIBBS_ATT");
		state = EnemyState::Attacking;
	}


	if (player_in_dmg_zone)
	{
		SetAnimation("RIBBS_ATT");
		state = EnemyState::Attacking;
	}

}

void Ribbs::UpdateAttackingState(float dt)
{
	if (!player_in_dmg_zone)
	{
		SetAnimation("RIBBS_IDLE");
		state = EnemyState::Idle;
	}
	if (animation->GetCurrentFrameNum() >= 6 &&
		player_in_dmg_zone)
	{
		PlaySound(SoundManager::sounds["splat4"]);
		GameScreen::player->take_dmg(10); 
	}
}

void Ribbs::UpdateHurtingState(float dt)
{
	if (AnimationEnded())
	{
		if (m_current_hp <= 0)
		{
			Die(1);
		}
		else
		{
			SetAnimation("RIBBS_IDLE");
			state = EnemyState::Idle;
		}

	}
}

void Ribbs::UpdateDyingState(float dt)
{
	taking_dmg = false;
	if (is_touching_floor)
	{
		m_body->SetEnabled(false);

	}
	if (AnimationEnded())
	{
		m_destroy = true;
		if (!LevelManager::world->IsLocked())
		{
			Rectangle r = { center_pos().x,center_pos().y, 13,30 };
			LevelManager::world->DestroyBody(m_body);
			LevelManager::level_entities_safe.push_back(std::make_unique<Leggy>(r));
			LevelManager::level_entities_safe.back().get()->m_draw_layers = 1;
			Leggy* l = reinterpret_cast<Leggy*>(LevelManager::level_entities_safe.back().get());
			l->state = EnemyState::Spawning;
			l->SetAnimation("LEGGY_STAND");
		}
			
	}
}
