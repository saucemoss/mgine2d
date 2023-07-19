#include "FlyingInfected.h"
#include "LevelManager.h"
#include "GameScreen.h"
#include "Util.h"
FlyingInfected::FlyingInfected(const Rectangle& rectangle) :
	Collidable({ rectangle.x,rectangle.y,20,20 }, b2_dynamicBody, FLYING_INF)
{

	InitAnimations();
	state = FlyingInfectedStates::Idle;


	// Add mappings for debug purposes
	StatesStrMap[FlyingInfectedStates::Idle] = "Idle";
	StatesStrMap[FlyingInfectedStates::Flying] = "Flying";
	StatesStrMap[FlyingInfectedStates::Attacking] = "Attacking";


	//Physics body cfg
	//add more mass 
	m_fixture->SetDensity(20.0f);
	m_circle.m_p.Set(0, 1.0f);
	m_body->ResetMassData();

	//player agro sensor
	b2PolygonShape agro_sesnor_box;
	agro_sesnor_box.SetAsBox(7.0f, 7.0f, b2Vec2(0.0, 0.0), 0);
	//fixture user data
	FixtureUserData* agroSensorName = new FixtureUserData;
	agroSensorName->name = "fi_agro";
	//fixture definition
	b2FixtureDef agroDef;
	agroDef.isSensor = true;
	agroDef.shape = &agro_sesnor_box;
	agroDef.userData.pointer = reinterpret_cast<uintptr_t>(agroSensorName);
	//create fixture using definition
	m_agro_sensor = m_body->CreateFixture(&agroDef);

	m_winghflap_sensor = util::SimpleSensor(m_body, "wingflap", 13.0f, 13.0f);


	//attack sensor
	b2PolygonShape attack_sesnor_box;
	attack_sesnor_box.SetAsBox(0.1f, 1.5f, b2Vec2(0.0, 1.2), 0);
	//fixture user data
	FixtureUserData* attackSensorName = new FixtureUserData;
	attackSensorName->name = "fi_att";
	//fixture definition
	b2FixtureDef attackDef;
	attackDef.isSensor = true;
	attackDef.shape = &attack_sesnor_box;
	attackDef.userData.pointer = reinterpret_cast<uintptr_t>(attackSensorName);
	//create fixture using definition
	m_attack_sensor = m_body->CreateFixture(&attackDef);
	m_body->SetLinearDamping(linear_dumping);
	EnitityManager::Add(this);

}

FlyingInfected::~FlyingInfected()
{
	EnitityManager::Remove(this);
}

void FlyingInfected::Update(float dt)
{

	SwitchFrames(dt);
	CheckPlayerTouch();
	CheckTouchGround();
	CheckAgroSensor();
	
	if (player_in_wingflap && !IsSoundPlaying(SoundManager::sounds["wing_flap"]) && (state != FlyingInfectedStates::Dying))
	{
		PlaySound(SoundManager::sounds["wing_flap"]);
	}

	if (state != FlyingInfectedStates::Dying)
	{
		m_body->ApplyForce(b2Vec2(0.0f, -world->GetGravity().y * m_body->GetMass()), m_body->GetWorldCenter(), true);
	}
	else
	{
		m_body->SetFixedRotation(false);
	}
	
	switch (state)
	{
	case FlyingInfectedStates::Idle:
		UpdateIdleState(dt);
		break;
	case FlyingInfectedStates::Flying:
		UpdateFlyingState(dt);
		break;
	case FlyingInfectedStates::Attacking:
		UpdateAttackingState(dt);
		break;
	case FlyingInfectedStates::Hurting:
		UpdateHurtingState(dt);
		break;
	case FlyingInfectedStates::Dying:
		UpdateDyingState(dt);
		break;
	}

}

void FlyingInfected::Die(int death_option)
{
	PlaySound(SoundManager::sounds["hurt4"]);
	SetAnimation("FLY_I_DEAD");
	state = FlyingInfectedStates::Dying;
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

void FlyingInfected::CheckTouchGround()
{
	is_touching_floor = false;
	if (ground_contacts > 0)
	{
		is_touching_floor = true;
	}
}

void FlyingInfected::CheckPlayerTouch()
{

	//if (animation->GetCurrentFrameNum() >= 6 &&
	//	player_in_dmg_zone &&
	//	state == FlyingInfectedStates::Attacking)
	//{
	//	GameScreen::player->Die();
	//}

}

void FlyingInfected::TakeDmg(int dmg)
{
	std::string dmgs[] = {"hit6","hit7","hit8"};
	SoundManager::PlayRandSounds(dmgs, 3);
	m_current_hp -= dmg;
	state = FlyingInfectedStates::Hurting;
	SetAnimation("FLY_I_DMG");

}

void FlyingInfected::set_velocity_x(float vx)
{
	m_body->SetLinearVelocity({
	vx,
	m_body->GetLinearVelocity().y,
		});
}

void FlyingInfected::set_velocity_y(float vy)
{
	m_body->SetLinearVelocity({
	m_body->GetLinearVelocity().x,
	vy,
		});
}

void FlyingInfected::set_velocity_xy(float vx, float vy)
{
	m_body->SetLinearVelocity({ vx, vy });
}

void FlyingInfected::Draw(int l)
{
	Rectangle cframe = looking_right ? CurrentFrame() : Rectangle{ CurrentFrame().x,
																	CurrentFrame().y,
																	CurrentFrame().width * -1,
																	CurrentFrame().height };
	auto spritePosX = center_pos().x - 10;
	auto spritePosY = center_pos().y - 1;
	auto angle = m_body->GetAngle();

	if (CurrentFrame().width == 96)
	{
		spritePosX = center_pos().x - 42;
		spritePosY = center_pos().y - 33;
	}

	Color c = WHITE;
	if (state == FlyingInfectedStates::Hurting)
	{
		c = RED;
	}

	

	DrawTexturePro(*animation->GetTexture(),
		cframe,
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ 0,0 },
		angle,
		c);

	//DrawText(std::to_string(solid_contacts).c_str(), center_pos().x - 50, center_pos().y - 40, 40, RED);
	//DrawText(std::to_string(m_current_hp).c_str(), center_pos().x, center_pos().y - 10, 10, RED);
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
		state = FlyingInfectedStates::Flying;
		SetAnimation("FLY_I_FLY");
	}
}

void FlyingInfected::UpdateFlyingState(float dt)
{
	if (!player_in_agro)
	{
		state = FlyingInfectedStates::Idle;
		SetAnimation("FLY_I_IDLE");
	}
	else
	{
		
		if (player_in_dmg_zone)
		{
			SetAnimation("FLY_I_ATT");
			PlaySound(SoundManager::sounds["splat8"]);
			state = FlyingInfectedStates::Attacking;
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
		state = FlyingInfectedStates::Idle;
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
			state = FlyingInfectedStates::Idle;
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
		if (!LevelManager::world->IsLocked())
			LevelManager::world->DestroyBody(m_body);
	}

}
