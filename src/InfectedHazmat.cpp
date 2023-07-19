#include "InfectedHazmat.h"
#include "LevelManager.h"
#include "GameScreen.h"
#include "Util.h"

InfectedHazmat::InfectedHazmat(const Rectangle& rectangle) :
	Collidable({ rectangle.x,rectangle.y,12,30 }, b2_dynamicBody, INFECTED_H)
{
	
	InitAnimations();
	state = InfectedHazmatState::Idle;


	// Add mappings for debug purposes
	StatesStrMap[InfectedHazmatState::Idle] = "Idle";
	StatesStrMap[InfectedHazmatState::Running] = "Running";
	StatesStrMap[InfectedHazmatState::Attacking] = "Attacking";


	//Physics body cfg
	//add more mass 
	m_fixture->SetDensity(60.0f);
	m_body->ResetMassData();

	//feet collider box
	b2PolygonShape feet_sesnor_box;
	feet_sesnor_box.SetAsBox(0.3f, 0.3f, b2Vec2(0, 0.80f), 0);
	//fixture user data
	FixtureUserData* feetFixtureName = new FixtureUserData;
	feetFixtureName->name = "ih_feet";
	//fixture definition
	b2FixtureDef feetDef;
	feetDef.isSensor = true;
	feetDef.shape = &feet_sesnor_box;
	feetDef.userData.pointer = reinterpret_cast<uintptr_t>(feetFixtureName);
	//create fixture using definition
	m_feet_sensor = m_body->CreateFixture(&feetDef);

	//player agro sensor
	b2PolygonShape agro_sesnor_box;
	agro_sesnor_box.SetAsBox(2.0f, 2.0f, b2Vec2(-3.0, 0.0), 0);
	//fixture user data
	FixtureUserData* agroSensorName = new FixtureUserData;
	agroSensorName->name = "ih_agro";
	//fixture definition
	b2FixtureDef agroDef;
	agroDef.isSensor = true;
	agroDef.shape = &agro_sesnor_box;
	agroDef.userData.pointer = reinterpret_cast<uintptr_t>(agroSensorName);
	//create fixture using definition
	m_agro_sensor = m_body->CreateFixture(&agroDef);

	
	b2PolygonShape agro_sesnor_box2;
	agro_sesnor_box2.SetAsBox(-2.0f, 2.0f, b2Vec2(3.0, 0.0), 0);
	//fixture user data
	FixtureUserData* agroSensorName2 = new FixtureUserData;
	agroSensorName2->name = "ih_agro";
	//fixture definition
	b2FixtureDef agroDef2;
	agroDef2.isSensor = true;
	agroDef2.shape = &agro_sesnor_box2;
	agroDef2.userData.pointer = reinterpret_cast<uintptr_t>(agroSensorName2);
	//create fixture using definition
	m_body->CreateFixture(&agroDef2);


	//proximity sensor (used for animation purposes)
	b2PolygonShape proximity_sesnor_box1;
	proximity_sesnor_box1.SetAsBox(0.2f, 0.2f, b2Vec2(2.0, 1.1), 0);
	//fixture user data
	FixtureUserData* proximitySensorName1 = new FixtureUserData;
	proximitySensorName1->name = "proxi";
	//fixture definition
	b2FixtureDef proximityDef1;
	proximityDef1.isSensor = true;
	proximityDef1.shape = &proximity_sesnor_box1;
	proximityDef1.userData.pointer = reinterpret_cast<uintptr_t>(proximitySensorName1);
	//create fixture using definition
	m_body->CreateFixture(&proximityDef1);
	b2PolygonShape proximity_sesnor_box2;
	proximity_sesnor_box2.SetAsBox(0.2f, 0.2f, b2Vec2(-2.0, 1.1), 0);
	//fixture user data
	FixtureUserData* proximitySensorName2 = new FixtureUserData;
	proximitySensorName2->name = "proxi";
	//fixture definition
	b2FixtureDef proximityDef2;
	proximityDef2.isSensor = true;
	proximityDef2.shape = &proximity_sesnor_box2;
	proximityDef2.userData.pointer = reinterpret_cast<uintptr_t>(proximitySensorName2);
	//create fixture using definition
	m_body->CreateFixture(&proximityDef2);

	//attack sensor
	b2PolygonShape attack_sesnor_box;
	attack_sesnor_box.SetAsBox(1.0f, 0.8f, b2Vec2(0.0, 0.0), 0);
	//fixture user data
	FixtureUserData* attackSensorName = new FixtureUserData;
	attackSensorName->name = "ih_att";
	//fixture definition
	b2FixtureDef attackDef;
	attackDef.isSensor = true;
	attackDef.shape = &attack_sesnor_box;
	attackDef.userData.pointer = reinterpret_cast<uintptr_t>(attackSensorName);
	//create fixture using definition
	m_attack_sensor = m_body->CreateFixture(&attackDef);

	//left&right collider boxes
	b2PolygonShape left_sesnor_box;
	left_sesnor_box.SetAsBox(0.05f, 0.61f, b2Vec2(-0.4f, 0), 0);
	b2PolygonShape right_sesnor_box;
	right_sesnor_box.SetAsBox(0.05f, 0.61f, b2Vec2(0.4f, 0), 0);
	//fixture user data
	FixtureUserData* right_sesnorName = new FixtureUserData;
	FixtureUserData* left_sesnorName = new FixtureUserData;
	right_sesnorName->name = "ih_r_s";
	left_sesnorName->name = "ih_l_s";
	//fixture definition
	b2FixtureDef left_sensor_def;
	left_sensor_def.isSensor = true;
	left_sensor_def.shape = &left_sesnor_box;
	left_sensor_def.userData.pointer = reinterpret_cast<uintptr_t>(left_sesnorName);
	b2FixtureDef right_sensor_def;
	right_sensor_def.isSensor = true;
	right_sensor_def.shape = &right_sesnor_box;
	right_sensor_def.userData.pointer = reinterpret_cast<uintptr_t>(right_sesnorName);
	//create fixture using definition
	m_left_sensor = m_body->CreateFixture(&left_sensor_def);
	m_right_sensor = m_body->CreateFixture(&right_sensor_def);
	m_body->SetLinearDamping(linear_dumping);
	EnitityManager::Add(this);
}

InfectedHazmat::~InfectedHazmat()
{
	EnitityManager::Remove(this);
}

void InfectedHazmat::Update(float dt)
{
	SwitchFrames(dt);
	CheckTouchGround();
	CheckAgroSensor();

	//std::cout << m_rectangle.x << std::endl;

	switch (state)
	{
	case InfectedHazmatState::Idle:
		UpdateIdleState(dt);
		break;
	case InfectedHazmatState::Running:
		UpdateRunningState(dt);
		break;
	case InfectedHazmatState::Attacking:
		UpdateAttackingState(dt);
		break;
	case InfectedHazmatState::Hurting:
		UpdateHurtingState(dt);
		break;
	case InfectedHazmatState::Dying:
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
	
	state = InfectedHazmatState::Dying;
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

void InfectedHazmat::CheckTouchGround()
{
	is_touching_floor = false;
	if (ground_contacts > 0)
	{
		is_touching_floor = true;
	}
}


void InfectedHazmat::TakeDmg(int dmg)
{
	std::string dmgs[] = { "hit2","hit3","hit4" };
	SoundManager::PlayRandSounds(dmgs, 3);
	m_current_hp -= dmg;
	state = InfectedHazmatState::Hurting;
	SetAnimation("IH_DMG");
	
}

void InfectedHazmat::set_velocity_x(float vx)
{
	m_body->SetLinearVelocity({
	vx,
	m_body->GetLinearVelocity().y,
		});
}

void InfectedHazmat::set_velocity_y(float vy)
{
	m_body->SetLinearVelocity({
	m_body->GetLinearVelocity().x,
	vy,
		});
}

void InfectedHazmat::set_velocity_xy(float vx, float vy)
{
	m_body->SetLinearVelocity({ vx, vy });
}

void InfectedHazmat::Draw(int l)
{
	Rectangle cframe = looking_right ? CurrentFrame() : Rectangle{  CurrentFrame().x,
																	CurrentFrame().y,
																	CurrentFrame().width * -1,
																	CurrentFrame().height };
	auto spritePosX = center_pos().x - 10;
	auto spritePosY = center_pos().y - 1 ;


	if (CurrentFrame().width==96)
	{
		spritePosX = center_pos().x-42;
		spritePosY = center_pos().y-33;
	}

	Color c = WHITE;
	if (state == InfectedHazmatState::Hurting)
	{
		c = RED;
	}

	DrawTexturePro(*animation->GetTexture(),
		cframe,
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ 0,0 },
		0.0f,
		c);

	//DrawText(std::to_string(m_current_hp).c_str(), center_pos().x, center_pos().y - 10, 10, RED);
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
		state = InfectedHazmatState::Running;
		SetAnimation("IH_RUN");
	}
}

void InfectedHazmat::UpdateRunningState(float dt)
{
	if (!player_in_agro)
	{
		state = InfectedHazmatState::Idle;
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
	state = InfectedHazmatState::Attacking;
}

void InfectedHazmat::UpdateAttackingState(float dt)
{

	if (!left_player_touch && !right_player_touch)
	{
		SetAnimation("IH_IDLE");
		state = InfectedHazmatState::Idle;
	}
	if (animation->GetCurrentFrameNum() >= 6 &&
		player_in_dmg_zone)
	{
		GameScreen::player->take_dmg(10);
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
			state = InfectedHazmatState::Idle;
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
		if (!LevelManager::world->IsLocked())
			LevelManager::world->DestroyBody(m_body);
	}

}
