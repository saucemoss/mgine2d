#include "InfectedHazmat.h"
#include "LevelManager.h"
#include "GameScreen.h"
#include "Util.h"

InfectedHazmat::InfectedHazmat(const Rectangle& rectangle) :
	Enemy({ rectangle.x,rectangle.y,12,26 }, INFECTED_H)
{
	
	InitAnimations();
	state = EnemyState::Idle;
	m_max_hp = 100;
	m_current_hp = m_max_hp;
	custom_pos = true;


	// Add mappings for debug purposes
	StatesStrMap[EnemyState::Idle] = "Idle";
	StatesStrMap[EnemyState::Running] = "Running";
	StatesStrMap[EnemyState::Attacking] = "Attacking";


	//Physics body cfg
	//add more mass 
	m_fixture->SetDensity(60.0f);
	m_body->ResetMassData();
	FixtureUserData* data = new FixtureUserData;
	data->tag = ENEMY_GROUP;
	m_fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(data);

	//feet collider box
	m_feet_sensor = util::SimpleSensor(m_body, "ih_feet", 0.3f, 0.3f, 0, 0.80f);

	//player agro sensor
	m_agro_sensor = util::SimpleSensor(m_body, "ih_agro", 2.0f, 2.0f, -3.0, 0.0);
	m_agro_sensor = util::SimpleSensor(m_body, "ih_agro", -2.0f, 2.0f, 3.0, 0.0);

	//proximity sensor (used for animation purposes)
	util::SimpleSensor(m_body, "proxi", 0.2f, 0.2f, 2.0, 1.1);
	util::SimpleSensor(m_body, "proxi", 0.2f, 0.2f,-2.0, 1.1);

	//attack sensor
	m_attack_sensor = util::SimpleSensor(m_body, "ih_att", 1.0f, 0.8f, 0.0, 0.0);

	//left&right collider boxes
	m_left_sensor = util::SimpleSensor(m_body, "ih_l_s", 0.05f, 0.80f, -0.6f, 0);
	m_right_sensor = util::SimpleSensor(m_body, "ih_r_s", 0.05f, 0.80f, 0.6f, 0);

	m_body->SetLinearDamping(linear_dumping);
}

InfectedHazmat::~InfectedHazmat()
{
	EnitityManager::Remove(this);
}

void InfectedHazmat::Update(float dt)
{
	SwitchFrames(dt);
	spritePosX = CurrentFrame().width == 96 ? spritePosX = center_pos().x - 42 : (int)center_pos().x - 10;
	spritePosY = CurrentFrame().width == 96 ? spritePosY = center_pos().y - 37 : (int)center_pos().y - 6;

	CheckTouchGround();
	CheckAgroSensor();

	//std::cout << m_rectangle.x << std::endl;

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

void InfectedHazmat::Die(int death_option)
{

	switch (death_option)
	{
	case 1:
		SetAnimation("IH_DEATH");
		PlaySound(SoundManager::sounds["hurt12"]);
		PlaySound(SoundManager::sounds["slime2short"]);
		break;
	case 2:
		solid_contacts < 2 ? SetAnimation("IH_DEATH3") : SetAnimation("IH_DEATH2");
		PlaySound(SoundManager::sounds["hurt9"]);
		PlaySound(SoundManager::sounds["splat5"]);
		PlaySound(SoundManager::sounds["splat1"]);
		PlaySound(SoundManager::sounds["slime1"]);
		break;
	}
	
	state = EnemyState::Dying;
}

void InfectedHazmat::CheckAgroSensor()
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


void InfectedHazmat::TakeDmg(int dmg)
{
	std::string dmgs[] = { "hit2","hit3","hit4" };
	SoundManager::PlayRandSounds(dmgs, 3);
	m_current_hp -= dmg;
	state = EnemyState::Hurting;
	SetAnimation("IH_DMG");
	
}

void InfectedHazmat::InitAnimations()
{
	animations->InitializeInfectedHAnimations();
	SetAnimation("IH_IDLE");
}

void InfectedHazmat::UpdateIdleState(float dt)
{
	if (player_in_agro)
	{
		state = EnemyState::Running;
		SetAnimation("IH_RUN");
	}
}

void InfectedHazmat::UpdateRunningState(float dt)
{
	if (!player_in_agro)
	{
		state = EnemyState::Idle;
		SetAnimation("IH_IDLE");
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

	if (left_player_touch && player_in_dmg_zone)
	{
		SetAttacking();
	}
	else if (left_player_touch)
	{
		looking_right = false;
	}
	
	if (right_player_touch && player_in_dmg_zone)
	{
		SetAttacking();
	}
	else if (right_player_touch)
	{
		looking_right = true;
	}
}

void InfectedHazmat::SetAttacking()
{
	SetAnimation("IH_ATT");
	PlaySound(SoundManager::sounds["slime1"]);
	state = EnemyState::Attacking;
}

void InfectedHazmat::UpdateAttackingState(float dt)
{

	if (!left_player_touch && !right_player_touch)
	{
		SetAnimation("IH_IDLE");
		state = EnemyState::Idle;
	}
	if (animation->GetCurrentFrameNum() >= 6 &&
		player_in_dmg_zone)
	{
		GameScreen::player->take_dmg(10);
		PlaySound(SoundManager::sounds["slime1"]);
	}
}

void InfectedHazmat::UpdateHurtingState(float dt)
{
	if (AnimationEnded())
	{
		if (m_current_hp < -100)
		{
			Die(2);
		}
		else if (m_current_hp <= 0)
		{
			Die(1);
		}
		else
		{
			SetAnimation("IH_IDLE");
			state = EnemyState::Idle;
		}

	}
}

void InfectedHazmat::UpdateDyingState(float dt)
{
	if (is_touching_floor)
	{
		m_body->SetEnabled(false);

	}
	if (AnimationEnded())
	{
		m_destroy = true;
		//if (!LevelManager::world->IsLocked())
		//	LevelManager::world->DestroyBody(m_body);
	}

}
