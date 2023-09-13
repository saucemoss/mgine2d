#include "Ivan.h"
#include "LevelManager.h"
#include "GameScreen.h"
#include "Util.h"
#include "BioBreath.h"

Ivan::Ivan(const Rectangle& rectangle, const ldtk::ArrayField<ldtk::IntPoint> path_points) :
	Enemy({ rectangle.x,rectangle.y,12,26 }, IVAN, b2_dynamicBody),
	m_path_points(path_points)
{
	if (m_path_points.size() > 0) m_next_point = m_path_points.at(0).value();
	InitAnimations();
	state = EnemyState::Idle;
	m_max_hp = 200;
	m_current_hp = m_max_hp;
	sprite_offset_96 = { -42 , -38 };
	sprite_offset_32 = { -10 ,-6 };

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
	util::SimpleSensor(m_body, "proxi", 0.2f, 0.2f, -2.0, 1.1);

	//attack sensor
	m_attack_sensor = util::SimpleSensor(m_body, "ih_att", 1.0f, 0.8f, 0.0, 0.0);

	//left&right collider boxes
	m_left_sensor = util::SimpleSensor(m_body, "ih_l_s", 0.05f, 0.80f, -0.6f, 0);
	m_right_sensor = util::SimpleSensor(m_body, "ih_r_s", 0.05f, 0.80f, 0.6f, 0);

	m_body->SetLinearDamping(linear_dumping);
}

Ivan::~Ivan()
{
	EnitityManager::Remove(this);
}

void Ivan::Update(float dt)
{
	SwitchFrames(dt);

	CheckTouchGround();
	//CheckAgroSensor();

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
	case EnemyState::Patrolling:
		UpdatePatrollingState(dt);
		break;
	}

}

void Ivan::Die(int death_option)
{

	switch (death_option)
	{
	case 1:
		SetAnimation("IVAN_DEAD");
		PlaySound(SoundManager::sounds["hurt12"]);
		PlaySound(SoundManager::sounds["slime2short"]);
		SpawnOrbs(1, center_pos());
		break;
	}

	state = EnemyState::Dying;
}

void Ivan::CheckAgroSensor()
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


void Ivan::TakeDmg(int dmg)
{
	std::string dmgs[] = { "hit2","hit3","hit4" };
	SoundManager::PlayRandSounds(dmgs, 3);
	m_current_hp -= dmg;
	bleed_particles();
	SpawnOrbs(1, center_pos());
	if (m_current_hp <= 0)
	{
		Die(1);
	}
}

void Ivan::InitAnimations()
{
	animations->InitializeIvanAnimations();
	SetAnimation("IVAN_IDLE");
}

void Ivan::UpdateIdleState(float dt)
{

	if (player_in_agro)
	{
		CheckAgroSensor();
		state = EnemyState::Running;
		SetAnimation("IVAN_RUN");
	}

	m_look_around_counter -= dt;
	if (m_look_around_counter <= 0.0f)
	{
		looking_right = !looking_right;
		m_look_around_counter = 0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f - 0.5f)));
	}

	m_idle_time_counter -= dt;
	if (m_idle_time_counter <= 0.0f && m_path_points.size() > 0)
	{
		m_idle_time_counter = 2.0f;
		if (m_path_step_counter < m_path_points.size() - 1)
		{
			m_path_step_counter++;
		}
		else
		{
			m_path_step_counter = 0;
		}
		state = EnemyState::Patrolling;
		SetAnimation("IVAN_RUN");
	}

}

void Ivan::UpdateRunningState(float dt)
{
	if (!player_in_agro)
	{
		state = EnemyState::Idle;
		SetAnimation("IVAN_IDLE");
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

void Ivan::SetAttacking()
{
	if (GetRandomValue(0, 1) == 0)
	{
		SetAnimation("IVAN_ATT1");
		PlaySound(SoundManager::sounds["slime1"]);
		state = EnemyState::Attacking;
	}
	else
	{
		SetAnimation("IVAN_ATT2");
		PlaySound(SoundManager::sounds["slime1"]);
		state = EnemyState::Attacking;
	}

}

void Ivan::UpdateAttackingState(float dt)
{
	if (animations->m_CurrentActiveAnimation == "IVAN_ATT1")
	{
		if (AnimationEnded() && (!left_player_touch && !right_player_touch) || !player_in_agro)
		{
			SetAnimation("IVAN_IDLE");
			state = EnemyState::Idle;
		}
		if (animation->GetCurrentFrameNum() >= 4 && animation->GetCurrentFrameNum() <= 10 &&
			player_in_dmg_zone)
		{
			GameScreen::player->take_dmg(10);
			PlaySound(SoundManager::sounds["slime1"]);
			Vector2 dir = { (looking_right ? 30 : -30), -15 };
			GameScreen::player->KnockBack(dir);
		}
	}
	else if (animations->m_CurrentActiveAnimation == "IVAN_ATT2")
	{
		int frame = animation->GetCurrentFrameNum();
		if (frame >= 11 && frame <= 16 && breath_attack_frame != frame)
		{
			Vector2 dir1 = { 0,0 };
			Vector2 dir2 = { 0,0 };
			Vector2 dir3 = { 0,0 };
			switch (frame)
			{
			case 11: 
				dir1 = { (looking_right ? 0.9f : -0.9f), 0.0f };
				dir2 = { (looking_right ? 0.9f : -0.9f), -0.1f };
				dir3 = { (looking_right ? 0.8f : -0.8f), -0.2f };
				   break;
			case 12: 
				dir1 = { (looking_right ? 0.7f : -0.7f), -0.4f };
				dir2 = { (looking_right ? 0.6f : -0.6f), -0.5f };
				dir3 = { (looking_right ? 0.5f : -0.5f), -0.6f };
				   break;
			case 13: 
				dir1 = { (looking_right ? 0.4f : -0.4f), -0.7f };
				dir2 = { (looking_right ? 0.3f : -0.3f), -0.9f };
				dir3 = { (looking_right ? 0.2f : -0.2f), -1.0f };
				   break;
			case 14: 
				dir1 = { (looking_right ? -0.4f : 0.4f), -0.7f };
				dir2 = { (looking_right ? -0.3f : 0.3f), -0.9f };
				dir3 = { (looking_right ? -0.2f : 0.2f), -1.0f };
				   break;
			case 15: 
				dir1 = { (looking_right ? -0.7f : 0.7f), -0.4f };
				dir2 = { (looking_right ? -0.6f : 0.6f), -0.5f };
				dir3 = { (looking_right ? -0.5f : 0.5f), -0.6f };
				   break;
			case 16: 
				dir1 = { (looking_right ? -0.9f : 0.9f), 0.0f };
				dir2 = { (looking_right ? -0.9f : 0.9f), -0.1f };
				dir3 = { (looking_right ? -0.8f : 0.8f), -0.2f };
				   break;
			}

			Rectangle rect = Rectangle{ center_pos().x , center_pos().y - 15, 16, 16 };
			LevelManager::level_entities_safe.push_back(std::make_unique<BioBreath>(rect, dir1));
			LevelManager::level_entities_safe.push_back(std::make_unique<BioBreath>(rect, dir2));
			LevelManager::level_entities_safe.push_back(std::make_unique<BioBreath>(rect, dir3));
			PlaySound(SoundManager::sounds["steam"]);
			breath_attack_frame = frame;

		
		}
		if (AnimationEnded())
		{
			SetAnimation("IVAN_IDLE");
			state = EnemyState::Idle;
		}
	}

}

void Ivan::UpdateHurtingState(float dt)
{

}

void Ivan::UpdateDyingState(float dt)
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

void Ivan::UpdatePatrollingState(float dt)
{

	float patroll_speed = 2.0f;
	m_next_point = m_path_points.at(m_path_step_counter).value();
	int x = pos().x;
	int y = center_pos().y;
	int px = m_next_point.x * settings::tileSize + settings::tileSize / 2;
	int py = m_next_point.y * settings::tileSize;
	m_body->SetLinearVelocity({ 0, 0 });
	if (px > x)
	{
		looking_right = true;
		m_body->SetLinearVelocity({ patroll_speed, m_body->GetLinearVelocity().y });
	}
	if (px < x)
	{
		looking_right = false;
		m_body->SetLinearVelocity({ -patroll_speed, m_body->GetLinearVelocity().y });
	}

	if (player_in_agro)
	{
		CheckAgroSensor();
		state = EnemyState::Running;
		SetAnimation("IVAN_RUN");
	}


	if ((int)px == (int)x)
	{
		state = EnemyState::Idle;
		SetAnimation("IVAN_IDLE");
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
