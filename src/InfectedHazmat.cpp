#include "InfectedHazmat.h"
#include "LevelManager.h"
#include "GameScreen.h"
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
	m_fixture->SetDensity(20.0f);
	m_body->ResetMassData();
	//feet collider box
	b2PolygonShape feet_sesnor_box;
	feet_sesnor_box.SetAsBox(0.3f, 0.3f, b2Vec2(0, 0.80f), 0);
	//fixture user data
	FixtureUserData* feetFixtureName = new FixtureUserData;
	feetFixtureName->name = "enemy_feet_sensor";
	//fixture definition
	b2FixtureDef feetDef;
	feetDef.isSensor = true;
	feetDef.shape = &feet_sesnor_box;
	feetDef.userData.pointer = reinterpret_cast<uintptr_t>(feetFixtureName);
	//create fixture using definition
	m_feet_sensor = m_body->CreateFixture(&feetDef);

	//player agro sensor
	b2PolygonShape agro_sesnor_box;
	agro_sesnor_box.SetAsBox(5.0f, 2.0f, b2Vec2(0.0, -1), 0);
	//fixture user data
	FixtureUserData* agroSensorName = new FixtureUserData;
	agroSensorName->name = "agro_sensor";
	//fixture definition
	b2FixtureDef agroDef;
	agroDef.isSensor = true;
	agroDef.shape = &agro_sesnor_box;
	agroDef.userData.pointer = reinterpret_cast<uintptr_t>(agroSensorName);
	//create fixture using definition
	m_agro_sensor = m_body->CreateFixture(&agroDef);

	//attack sensor
	b2PolygonShape attack_sesnor_box;
	attack_sesnor_box.SetAsBox(1.0f, 0.8f, b2Vec2(0.0, 0.0), 0);
	//fixture user data
	FixtureUserData* attackSensorName = new FixtureUserData;
	attackSensorName->name = "attack_sensor";
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
	right_sesnorName->name = "enemy_right_sensor";
	left_sesnorName->name = "enemy_left_sensor";
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
	CheckPlayerTouch();
	CheckTouchGround();
	CheckAgroSensor();
	CheckIfAxed();



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

void InfectedHazmat::Die()
{
	m_body->SetEnabled(false);
	SetAnimation("IH_DEATH");
	state = InfectedHazmatState::Dying;
}

void InfectedHazmat::CheckAgroSensor()
{
	player_in_agro = LevelManager::CheckPlayerInSensor(*m_agro_sensor);
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
	if (m_body->GetContactList() != nullptr)
	{
		auto con = m_body->GetContactList()->contact;
		while (con != nullptr)
		{
			auto obj1 = reinterpret_cast<FixtureUserData*>(con->GetFixtureA()->GetUserData().pointer);
			auto obj2 = reinterpret_cast<FixtureUserData*>(con->GetFixtureB()->GetUserData().pointer);
			if (obj1 != nullptr && obj1->name == "enemy_feet_sensor" && con->IsTouching())
			{
				is_touching_floor = true;
			}
			if (obj2 != nullptr && obj2->name == "enemy_feet_sensor" && con->IsTouching())
			{
				is_touching_floor = true; 
			}
			con = con->GetNext();
		}
	}
}

void InfectedHazmat::CheckPlayerTouch()
{
	left_player_touch = LevelManager::CheckPlayerInSensor(*m_left_sensor);
	right_player_touch = LevelManager::CheckPlayerInSensor(*m_right_sensor);
}

void InfectedHazmat::CheckIfAxed()
{
	if (LevelManager::CheckAxeInSensor(*m_fixture) &&
		GameScreen::player->axe != nullptr)
	{
		axe_vel = fabs(GameScreen::player->axe->m_body->GetLinearVelocity().x);
	}
	if (LevelManager::CheckAxeInSensor(*m_fixture) &&
		GameScreen::player->axe != nullptr &&
		(fabs(GameScreen::player->axe->m_body->GetLinearVelocity().x) > 5.0f ||
		 fabs(GameScreen::player->axe->m_body->GetLinearVelocity().y) > 5.0f))
	{
		Die();
	}

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

void InfectedHazmat::Draw()
{
	Rectangle cframe = looking_right ? CurrentFrame() : Rectangle{ CurrentFrame().x,
																CurrentFrame().y,
																CurrentFrame().width * -1,
																CurrentFrame().height };
	auto spritePosX = center_pos().x - 10;
	auto spritePosY = center_pos().y - 2 ;
	DrawTexturePro(*animation->GetTexture(),
		cframe,
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ 0,0 },
		0.0f,
		WHITE);

	if (animation->GetCurrentFrameNum() >= 6 && 
		LevelManager::CheckPlayerInSensor(*m_attack_sensor) &&
		state == InfectedHazmatState::Attacking)
	{
		DrawText("DEAD!", center_pos().x-50, center_pos().y-40, 40, RED);
	}
	if (LevelManager::CheckAxeInSensor(*m_fixture) &&
		GameScreen::player->axe != nullptr)
	{
		DrawText("Contact!", center_pos().x - 50, center_pos().y - 40, 40, RED);
	}

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

	if (left_player_touch)
	{
		looking_right = false;
		SetAnimation("IH_ATT");
		state = InfectedHazmatState::Attacking;
	}
	else 
	if (right_player_touch)
	{
		looking_right = true;
		SetAnimation("IH_ATT");
		state = InfectedHazmatState::Attacking;
	}
}

void InfectedHazmat::UpdateAttackingState(float dt)
{
	if (!left_player_touch && !right_player_touch)
	{
		SetAnimation("IH_IDLE");
		state = InfectedHazmatState::Idle;
	}
	if (IsKeyPressed(KEY_E))
	{
		SetAnimation("IH_DMG");
		state = InfectedHazmatState::Hurting;
	}
	if (IsKeyPressed(KEY_F))
	{
		Die();
	}
}

void InfectedHazmat::UpdateHurtingState(float dt)
{
	if (AnimationEnded())
	{
		PlayOnceUninterupt("IH_IDLE");
		state = InfectedHazmatState::Idle;
	}
}

void InfectedHazmat::UpdateDyingState(float dt)
{
	if (AnimationEnded())
	{
		m_destroy = true;
		LevelManager::world->DestroyBody(m_body);
	}
}
