#include "FlyingInfected.h"
#include "LevelManager.h"
#include "GameScreen.h"
#include "Util.h"
FlyingInfected::FlyingInfected(const Rectangle& rectangle) :
	Enemy({ rectangle.x,rectangle.y,20,20 }, FLYING_INF)
{
	speed = 4.0f;
	m_max_hp = 150;
	m_current_hp = m_max_hp;
	InitAnimations();
	state = EnemyState::Idle;

	sprite_offset_96 = {- 42 , -33 };

	//Physics body cfg
	//add more mass 
	FixtureUserData* data = new FixtureUserData;
	data->tag = ENEMY_GROUP;
	m_fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(data);
	m_fixture->SetDensity(20.0f);
	m_circle.m_p.Set(0, 1.0f);
	m_body->ResetMassData();

	//player agro sensor
	m_agro_sensor = util::SimpleSensor(m_body, "fi_agro", 7.0f, 7.0f, 0.0f, 0.0f);
	m_winghflap_sensor = util::SimpleSensor(m_body, "wingflap", 25.0f, 25.0f);
	m_attack_sensor = util::SimpleSensor(m_body, "fi_att", 0.1f, 1.5f, 0.0f, 1.2f);
	m_body->SetLinearDamping(linear_dumping);

	my_wingflap_sound = LoadSound("res/sound/monster/wings_flap.wav");
}

FlyingInfected::~FlyingInfected()
{
	UnloadSound(my_wingflap_sound);
	EnitityManager::Remove(this);
}

void FlyingInfected::Update(float dt)
{

	SwitchFrames(dt);
	CheckTouchGround();
	CheckAgroSensor();
	
	if (state != EnemyState::Dying)
	{
		float vol = 0.7f - Vector2DistanceSqr(pos(), GameScreen::player->pos()) * 0.00001f;
		if (vol < 0.0f) vol = 0.0f;
		SetSoundVolume(my_wingflap_sound, vol);
		if (!IsSoundPlaying(my_wingflap_sound))
		{
			PlaySound(my_wingflap_sound);
		}
	}

	if (state != EnemyState::Dying)
	{
		m_body->ApplyForce(b2Vec2(0.0f, -world->GetGravity().y * m_body->GetMass()), m_body->GetWorldCenter(), true);
	}
	else
	{
		m_body->SetFixedRotation(false);
	}
	
	switch (state)
	{
	case EnemyState::Idle:
		UpdateIdleState(dt);
		break;
	case EnemyState::Flying:
		UpdateFlyingState(dt);
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

void FlyingInfected::Die(int death_option)
{
	PlaySound(SoundManager::sounds["hurt4"]);
	SetAnimation("FLY_I_DEAD");
	state = EnemyState::Dying;
}

void FlyingInfected::CheckAgroSensor()
{

	if (player_in_agro && GameScreen::player->center_pos().x > center_pos().x)
	{
		looking_right = true;
	}
	else if (player_in_agro && GameScreen::player->center_pos().x < center_pos().x)
	{
		looking_right = false;
	}
}



void FlyingInfected::TakeDmg(int dmg)
{
	std::string dmgs[] = {"hit6","hit7","hit8"};
	SoundManager::PlayRandSounds(dmgs, 3);
	m_current_hp -= dmg;
	state = EnemyState::Hurting;
	SetAnimation("FLY_I_DMG");
	bleed_particles();

}


void FlyingInfected::InitAnimations()
{
	animations->InitializeFlyingInfectedAnimations();
	SetAnimation("FLY_I_IDLE");
}

void FlyingInfected::UpdateIdleState(float dt)
{
	if (player_in_agro)
	{
		state = EnemyState::Flying;
		SetAnimation("FLY_I_FLY");
	}
}

void FlyingInfected::UpdateFlyingState(float dt)
{
	if (!player_in_agro)
	{
		state = EnemyState::Idle;
		SetAnimation("FLY_I_IDLE");
	}
	else
	{
		
		if (player_in_dmg_zone)
		{
			SetAnimation("FLY_I_ATT");
			PlaySound(SoundManager::sounds["splat8"]);
			state = EnemyState::Attacking;
		}

		Vector2 over_player_head = { GameScreen::player->center_pos().x, GameScreen::player->center_pos().y - 40 };
		if (center_pos().x < over_player_head.x - 10)
		{
			set_velocity_x(speed);
		}
		else if (center_pos().x > over_player_head.x + 10)
		{
			set_velocity_x(-speed);
		}
		if (center_pos().y < over_player_head.y - 10)
		{
			set_velocity_y(speed);
		}
		else if(center_pos().y > over_player_head.y + 10)
		{
			set_velocity_y(-speed);
		}



	}
}

void FlyingInfected::UpdateAttackingState(float dt)
{
	if (!player_in_dmg_zone)
	{
		SetAnimation("FLY_I_IDLE");
		state = EnemyState::Idle;
	}

	if (animation->GetCurrentFrameNum() > 2 &&
		player_in_dmg_zone)
	{
		GameScreen::player->take_dmg(10);
	}
}

void FlyingInfected::UpdateHurtingState(float dt)
{

	if (GameScreen::player->looking_right)
	{
		m_body->ApplyForce(b2Vec2(400.0f, -600.0f), m_body->GetWorldCenter(), true);
	}
	else
	{
		m_body->ApplyForce(b2Vec2(-400.0f, -600.0f), m_body->GetWorldCenter(), true);
	}
	
	
	if (AnimationEnded())
	{
		if (m_current_hp < -50)
		{
			Die(2);
		}
		else if (m_current_hp <= 0)
		{
			Die(1);
		}
		else
		{
			SetAnimation("FLY_I_IDLE");
			state = EnemyState::Idle;
		}

	}
}

void FlyingInfected::UpdateDyingState(float dt)
{
	if (is_touching_floor)
	{
		m_body->SetFixedRotation(false);
		m_body->SetEnabled(false);
	}
	if (AnimationEnded())
	{
		m_destroy = true;
		//if (!LevelManager::world->IsLocked())
		//	LevelManager::world->DestroyBody(m_body);
	}
}

void FlyingInfected::UpdateRunningState(float dt)
{
}
