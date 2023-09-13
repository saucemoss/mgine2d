#include "HeadSpit.h"
#include "SoundManager.h"
#include "GameScreen.h"
#include "BioBomb.h"

HeadSpit::HeadSpit(const Rectangle& rectangle) : Enemy({ rectangle.x, rectangle.y, 10, 20 }, HSPIT, b2_kinematicBody)
{
	sprite_offset_32 = { -10,-13 };
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
	util::SimpleCircleSensor(m_body, "hspit_att", 150.0f, 0.0f, 0.0f);
}

HeadSpit::~HeadSpit()
{
	EnitityManager::Remove(this);
}

void HeadSpit::Update(float dt)
{
	if (upsidedown && !up_switched)
	{
		sprite_rotation = 180.0f;
		sprite_offset_32 = { 32,32 }; 
		m_body->SetTransform({ m_body->GetPosition().x - 16.0f/settings::PPM, m_body->GetPosition().y - 16.0f / settings::PPM }, 0.0f);
		up_switched = true;
	}

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
		SpawnOrbs(2, center_pos());
	}
}

void HeadSpit::UpdateIdleState(float dt)
{
	if (player_in_dmg_zone)
	{
		attack_counter -= dt;
		if (attack_counter <= 0.0f)
		{
			SetAnimation("HSPIT_ATT");
			state = EnemyState::Attacking;
		}
	}

}


void HeadSpit::UpdateAttackingState(float dt)
{
	if (animation->GetCurrentFrameNum() == 0 && shot)
	{
		shot = false;
		SetAnimation("HSPIT_IDLE");
		state = EnemyState::Idle;
		attack_counter = 1.0f;
	}

	if (!player_in_dmg_zone && AnimationEnded())
	{
		SetAnimation("HSPIT_IDLE");
		state = EnemyState::Idle;
		shot = false;
	}
	else if (animation->GetCurrentFrameNum() >= 3 &&
		player_in_dmg_zone && !shot)
	{
		shot = true;
		PlaySound(SoundManager::sounds["splat4"]);

		Rectangle rect = Rectangle{ center_pos().x , center_pos().y + (upsidedown? 8.0f: -8.0f) , 16, 16};
		LevelManager::level_entities_safe.push_back(std::make_unique<BioBomb>(rect));
		BioBomb* bomb = reinterpret_cast<BioBomb*>(LevelManager::level_entities_safe.back().get());

		float m_size_min_x = -4.0f;
		float m_size_max_x = 4.0f;
		float m_size_min_y = upsidedown ? 4.0f : -4.0f;
		float m_size_max_y = upsidedown ? 16.0f : -16.0f;
		float posX = m_size_min_x / 2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_size_max_x / 2 - m_size_min_x / 4)));
		float posY = m_size_min_y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_size_max_y - m_size_min_y)));
		Vector2 random_spread_impulse = { posX,posY };

		bomb->random_spread_impulse = random_spread_impulse;

		ParticleEmitter* p = new ParticleEmitter({pos().x, pos().y - (upsidedown? -10.0f : 10.0f)});
		GameScreen::Particles->Add(DefinedEmitter::acid_head_burst, p);
		p->EmitParticles();

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


