#include "WhiteCoatArm.h"
#include "LevelManager.h"
#include "GameScreen.h"
#include "Util.h"

WhiteCoatArm::WhiteCoatArm(const Rectangle& rectangle) :
	Enemy({ rectangle.x,rectangle.y+8,12,26 }, WCARM, b2_kinematicBody)
{

	InitAnimations();
	state = EnemyState::Idle;
	m_max_hp = 100;
	m_current_hp = m_max_hp;

	sprite_offset_32 = {-10,-6};
	sprite_offset_96 = { -42, -37 };
	sprite_offset_224 = { -100,-102 };


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
	m_feet_sensor = util::SimpleSensor(m_body, "wca_feet", 0.3f, 0.3f, 0, 0.80f);

	//player agro sensor
	m_agro_sensor = util::SimpleSensor(m_body, "wca_agro", 3.3f, 2.0f, -3.3, 0.0);
	m_agro_sensor = util::SimpleSensor(m_body, "wca_agro", -3.3f, 2.0f, 3.3, 0.0);

	//attack sensor
	m_attack_sensor = util::SimpleSensor(m_body, "wca_att", 6.6f, 0.2f, 0.0, 0.0);

	//left&right collider boxes
	m_left_sensor = util::SimpleSensor(m_body, "wca_l_s", 0.05f, 0.80f, -0.6f, 0);
	m_right_sensor = util::SimpleSensor(m_body, "wca_r_s", 0.05f, 0.80f, 0.6f, 0);

	m_body->SetLinearDamping(linear_dumping);
}

WhiteCoatArm::~WhiteCoatArm()
{
	EnitityManager::Remove(this);
}

void WhiteCoatArm::Update(float dt)
{
	SwitchFrames(dt);
	CheckTouchGround();
	CheckAgroSensor();

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

void WhiteCoatArm::Die(int death_option)
{

	SetAnimation("WCARM_DEAD");
	state = EnemyState::Dying;
	PlaySound(SoundManager::sounds["hurt7"]);
}

void WhiteCoatArm::CheckAgroSensor()
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


void WhiteCoatArm::TakeDmg(int dmg)
{
	std::string dmgs[] = { "hit2","hit3","hit4" };
	SoundManager::PlayRandSounds(dmgs, 3);
	PlaySound(SoundManager::sounds["hurt6"]);
	m_current_hp -= dmg;
	state = EnemyState::Hurting;
	SetAnimation("WCARM_HURT");
	bleed_particles();

}

void WhiteCoatArm::InitAnimations()
{
	animations->InitializeWCArmAnimations();
	SetAnimation("WCARM_IDLE");
}

void WhiteCoatArm::UpdateIdleState(float dt)
{
	if (player_in_agro)
	{
		SetAttacking();
	}
}

void WhiteCoatArm::UpdateRunningState(float dt)
{

}

void WhiteCoatArm::SetAttacking()
{
	SetAnimation("WCARM_ATT");
	
	state = EnemyState::Attacking;
}

void WhiteCoatArm::UpdateAttackingState(float dt)
{

	if (!player_in_agro && AnimationEnded())
	{
		
		SetAnimation("WCARM_IDLE");
		state = EnemyState::Idle;
	}

	if (animation->GetCurrentFrameNum() >= 5)
	{
		if (!IsSoundPlaying(SoundManager::sounds["monster-1"])) PlaySound(SoundManager::sounds["monster-1"]);
		
	}

	if (animation->GetCurrentFrameNum() >= 6 &&
		player_in_dmg_zone)
	{
		GameScreen::player->take_dmg(10);
		PlaySound(SoundManager::sounds["splat5"]);
	}
}

void WhiteCoatArm::UpdateHurtingState(float dt)
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
			SetAnimation("WCARM_IDLE");
			state = EnemyState::Idle;
		}

	}
}

void WhiteCoatArm::UpdateDyingState(float dt)
{
	if (is_touching_floor)
	{
		m_body->SetEnabled(false);

	}
	if (AnimationEnded())
	{
		m_destroy = true;
	}

}
