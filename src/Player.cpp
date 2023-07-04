#include "Player.h"
#include "Settings.h"
#include "Animations.h"
#include <iostream>
#include "Settings.h"
#include "GameScreen.h"
#include "Util.h"
#include "SolidTile.h"
#include "raymath.h"
#include <algorithm>
#include "LevelManager.h"
#include "WoodCrate.h"
#include "FireAxe.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

FireAxe* Player::axe = nullptr;

Player::Player()
	:
	Collidable({50,250,12,20}, b2_dynamicBody, PLAYER)
{
	NewBody();
	InitAnimations();
	state = PlayerState::Idle;
	


	// Add mappings for debug purposes
	StatesStrMap[PlayerState::Idle] = "Idle";
	StatesStrMap[PlayerState::Running] = "Running";
	StatesStrMap[PlayerState::Jumping] = "Jumping";
	StatesStrMap[PlayerState::Falling] = "Falling";
	ColStrMap[ColliderTag::DOOR] = "DOOR";
	ColStrMap[ColliderTag::ELEVATOR] = "ELEVATOR";
	ColStrMap[ColliderTag::ELEVATOR_CALL_SW] = "ELEVATOR_CALL_SW";
	ColStrMap[ColliderTag::LEVEL_PORTAL] = "LEVEL_PORTAL";
	ColStrMap[ColliderTag::M_BLOCK] = "M_BLOCK";
	ColStrMap[ColliderTag::PLAYER] = "PLAYER";
	ColStrMap[ColliderTag::SOLID] = "SOLID";
	ColStrMap[ColliderTag::UNASSIGNED] = "UNASSIGNED";


	//Shader test
	shdrOutline = LoadShader(0, TextFormat("res/shaders/glsl%i/outline.fs", GLSL_VERSION));
	// Get shader locations
	int outlineSizeLoc = GetShaderLocation(shdrOutline, "outlineSize");
	int outlineColorLoc = GetShaderLocation(shdrOutline, "outlineColor");
	int textureSizeLoc = GetShaderLocation(shdrOutline, "textureSize");

	// Set shader values (they can be changed later)
	SetShaderValue(shdrOutline, outlineSizeLoc, &outlineSize, SHADER_UNIFORM_FLOAT);
	SetShaderValue(shdrOutline, outlineColorLoc, outlineColor, SHADER_UNIFORM_VEC4);
	SetShaderValue(shdrOutline, textureSizeLoc, textureSize, SHADER_UNIFORM_VEC2);


}

void Player::NewBody()
{
	//feet collider box
	b2PolygonShape feet_sesnor_box;
	feet_sesnor_box.SetAsBox(0.3f, 0.3f, b2Vec2(0, 0.80f), 0);
	//fixture user data
	FixtureUserData* feetFixtureName = new FixtureUserData;
	feetFixtureName->name = "feet_sensor";
	//fixture definition
	b2FixtureDef feetDef;
	feetDef.isSensor = true;
	feetDef.shape = &feet_sesnor_box;
	feetDef.userData.pointer = reinterpret_cast<uintptr_t>(feetFixtureName);
	//create fixture using definition
	m_feet_sensor = m_body->CreateFixture(&feetDef);

	//left&right collider boxes
	b2PolygonShape left_sesnor_box;
	left_sesnor_box.SetAsBox(0.05f, 0.61f, b2Vec2(-0.4f, 0), 0);
	b2PolygonShape right_sesnor_box;
	right_sesnor_box.SetAsBox(0.05f, 0.61f, b2Vec2(0.4f, 0), 0);
	//fixture user data
	FixtureUserData* right_sesnorName = new FixtureUserData;
	FixtureUserData* left_sesnorName = new FixtureUserData;
	right_sesnorName->name = "right_sensor";
	left_sesnorName->name = "left_sensor";
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

	//generously give axe
	m_has_axe = true;
}

Player::~Player()
{
	EnitityManager::Remove(this);
}

void Player::Update(float dt)
{
	SetShaderValue(shdrOutline, outlineSizeLoc, &outlineSize, SHADER_UNIFORM_FLOAT);

	// Swtich animation frames for current anim
	SwitchFrames(dt);

	LevelPortalCheck();

	CheckTouchGround();

	CheckWallTouch();

	CheckAxeTouch();

	CheckThrowAxe();

	if (GameScreen::debug)
	{
		contact_debug_str = "";
		if (m_body->GetContactList() != nullptr)
		{
			auto con = m_body->GetContactList()->contact;
			while (con != nullptr)
			{
				auto obj1 = reinterpret_cast<Collidable*>(con->GetFixtureA()->GetBody()->GetUserData().pointer);
				auto obj2 = reinterpret_cast<Collidable*>(con->GetFixtureB()->GetBody()->GetUserData().pointer);
				contact_debug_str += ColStrMap[obj1->m_colliderTag];
				contact_debug_str += ", ";
				contact_debug_str += ColStrMap[obj2->m_colliderTag];
				contact_debug_str += "\n";
				con = con->GetNext();
			}
		}
	}


	// Update player in one possible state
	switch (state)
	{
	case PlayerState::Idle:
		UpdateIdleState(dt);
		break;
	case PlayerState::Running:
		UpdateRunningState(dt);
		break;
	case PlayerState::Jumping:
		UpdateJumpingState(dt);
		break;
	case PlayerState::Falling:
		UpdateFallingState(dt);
		break;
	}
}

void Player::LevelPortalCheck()
{
	if (m_body->GetContactList() != nullptr)
	{
		auto con = m_body->GetContactList()->contact;
		while (con != nullptr)
		{
			auto obj1 = reinterpret_cast<Collidable*>(con->GetFixtureA()->GetBody()->GetUserData().pointer);
			auto obj2 = reinterpret_cast<Collidable*>(con->GetFixtureB()->GetBody()->GetUserData().pointer);
			if ((obj1 != nullptr && obj2 != nullptr && obj1->m_colliderTag == LEVEL_PORTAL && obj2->m_colliderTag == PLAYER && con->IsTouching()) ||
				(obj2 != nullptr && obj1 != nullptr && obj2->m_colliderTag == LEVEL_PORTAL && obj1->m_colliderTag == PLAYER && con->IsTouching()))
			{
				LevelPortal* lpptr = nullptr;
				if (obj1->m_colliderTag == LEVEL_PORTAL)
				{
					lpptr = static_cast<LevelPortal*>(obj1);
				}
				else
				{
					lpptr = static_cast<LevelPortal*>(obj2);
				}
				auto ref = lpptr->m_iid_reference;
				if (lpptr->is_active)
				{
					
					GameScreen::LevelMgr->LoadLevel(lpptr->m_to_level);
					const ldtk::Entity& connected_portal = GameScreen::LevelMgr->currentLdtkLevel->getLayer("Entities").getEntity(ref);
					Vector2 newPos{ (connected_portal.getPosition().x ) ,
									(connected_portal.getPosition().y ) };
					GameScreen::camera.target = newPos;
					m_body->SetTransform({ newPos.x / settings::PPM, newPos.y / settings::PPM }, 0);
					break;
				}
			}
			con = con->GetNext();
		}
	}
}

void Player::CheckTouchGround()
{
	is_touching_floor = false;
	if (m_body->GetContactList() != nullptr)
	{
		auto con = m_body->GetContactList()->contact;
		while (con != nullptr)
		{
			auto obj1 = reinterpret_cast<FixtureUserData*>(con->GetFixtureA()->GetUserData().pointer);
			auto obj2 = reinterpret_cast<FixtureUserData*>(con->GetFixtureB()->GetUserData().pointer);
			if (obj1 != nullptr && obj1->name == "feet_sensor" && con->IsTouching())
			{
				is_touching_floor = true;
			}
			if (obj2 != nullptr && obj2->name == "feet_sensor" && con->IsTouching())
			{
				is_touching_floor = true;
			}
			con = con->GetNext();
		}
	}
}

void Player::CheckWallTouch()
{
	//this seems hacky and tragic honestly
	left_wall_touch = false;
	right_wall_touch = false;

	if (m_body->GetContactList() != nullptr)
	{
		auto con = m_body->GetContactList()->contact;
		while (con != nullptr)
		{
			auto obj1 = reinterpret_cast<FixtureUserData*>(con->GetFixtureA()->GetUserData().pointer);
			if (obj1 != nullptr && obj1->name == "left_sensor" && con->IsTouching())
			{
				auto obj2 = reinterpret_cast<Collidable*>(con->GetFixtureB()->GetBody()->GetUserData().pointer);
				if (obj2 != nullptr && (obj2->m_colliderTag == SOLID || obj2->m_colliderTag == M_BLOCK))
				{
					left_wall_touch = true;
				}	
			}
			if (obj1 != nullptr && obj1->name == "right_sensor" && con->IsTouching())
			{
				auto obj2 = reinterpret_cast<Collidable*>(con->GetFixtureB()->GetBody()->GetUserData().pointer);
				if (obj2 != nullptr && (obj2->m_colliderTag == SOLID || obj2->m_colliderTag == M_BLOCK))
				{
					right_wall_touch = true;
				}
			}
			obj1 = reinterpret_cast<FixtureUserData*>(con->GetFixtureB()->GetUserData().pointer);
			if (obj1 != nullptr && obj1->name == "left_sensor" && con->IsTouching())
			{
				auto obj2 = reinterpret_cast<Collidable*>(con->GetFixtureA()->GetBody()->GetUserData().pointer);
				if (obj2 != nullptr && (obj2->m_colliderTag == SOLID || obj2->m_colliderTag == M_BLOCK))
				{
					left_wall_touch = true;
				}
			}
			if (obj1 != nullptr && obj1->name == "right_sensor" && con->IsTouching())
			{
				auto obj2 = reinterpret_cast<Collidable*>(con->GetFixtureA()->GetBody()->GetUserData().pointer);
				if (obj2 != nullptr && (obj2->m_colliderTag == SOLID || obj2->m_colliderTag == M_BLOCK))
				{
					right_wall_touch = true;
				}
			}

			con = con->GetNext();
		}
	}
}

void Player::CheckAxeTouch()
{
	if (!m_has_axe)
	{
		m_has_axe = LevelManager::CheckAxeInSensor(*m_fixture);
		if (m_has_axe)
		{
			LevelManager::RemoveAxeFromLevel(*axe);
		}
	}
}

void Player::CheckThrowAxe()
{
	if (IsKeyPressed(KEY_SPACE) && m_has_axe)
	{

		Rectangle rect = looking_right ? Rectangle{ pos().x + 16,
													pos().y - 16,
													6,
													6 }
										: Rectangle{pos().x - 16,
													pos().y - 16,
													6,
													6 };

		LevelManager::level_entities_safe.push_back(std::make_unique<FireAxe>(rect));
		axe = reinterpret_cast<FireAxe*>(LevelManager::level_entities_safe.back().get());
		axe->m_body->ApplyAngularImpulse(3.5f, true);
		if (looking_right)
		{
			axe->m_body->ApplyLinearImpulseToCenter({ 140,-22 }, true);
		}
		else
		{
			axe->m_body->ApplyLinearImpulseToCenter({ -140,-22 }, true);
		}
		m_has_axe = false;
	}
}

void Player::set_velocity_x(float vx)
{

	if (vx > 0 && right_wall_touch)
	{
		vx = 0.0f;
	}
	if (vx < 0 && left_wall_touch)
	{
		vx = 0.0f;
	}

	m_body->SetLinearVelocity({
		vx,
		m_body->GetLinearVelocity().y,
		});
}

void Player::set_velocity_y(float vy)
{
	m_body->SetLinearVelocity({
		m_body->GetLinearVelocity().x,
		vy,
		});
}

void Player::set_velocity_xy(float vx, float vy)
{
	m_body->SetLinearVelocity({ vx, vy });
}

void Player::Draw()
{

	Rectangle cframe = looking_right ? CurrentFrame() : Rectangle{  CurrentFrame().x,
																	CurrentFrame().y,
																	CurrentFrame().width * -1,
																	CurrentFrame().height};
	auto spritePosX = center_pos().x - 10;
	auto spritePosY = center_pos().y - 12;

	if(animations->m_CurrentActiveAnimation=="P_ATT1")
	{
		spritePosX = center_pos().x - 24;
		spritePosY = center_pos().y - 28;
	}
	


	//BeginShaderMode(shdrOutline);
	DrawTexturePro(*animation->GetTexture(),
		cframe,
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ 0,0 },
		0.0f,
		WHITE);

	//EndShaderMode();
}

void Player::InitAnimations()
{
	animations->InitializePlayerAnimations();
	SetAnimation("P_IDLE");
}

void Player::UpdateIdleState(float dt)
{
	if (IsKeyPressed(KEY_UP) && is_touching_floor)
	{
		set_velocity_y(-jump_force);
		state = PlayerState::Jumping;
		SetAnimation("P_JUMP");
	}
	else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT))
	{
		SetAnimation("P_RUN");
		state = PlayerState::Running;
	}

	if (m_body->GetLinearVelocity().y >= 0 && !is_touching_floor)
	{
		state = PlayerState::Falling;
		SetAnimation("P_FALL");
	}

}

void Player::UpdateRunningState(float dt)
{
	if (IsKeyPressed(KEY_UP) && (is_touching_floor || coyote_time_counter > 0))
	{
		set_velocity_y(-jump_force);
		state = PlayerState::Jumping;
		SetAnimation("P_JUMP");
	}
	else if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
	{
		state = PlayerState::Idle;
		set_velocity_x(0.0f);
		SetAnimation("P_IDLE");
	}
	else if (IsKeyDown(KEY_LEFT))
	{
		state = PlayerState::Running;
		set_velocity_x(-speed);
		looking_right = false;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		state = PlayerState::Running;
		set_velocity_x(speed);
		looking_right = true;
	}


	if (m_body->GetLinearVelocity().y >= 0 && !is_touching_floor)
	{
		state = PlayerState::Falling;
		SetAnimation("P_FALL");
	}

}

void Player::UpdateJumpingState(float dt)
{
	
	coyote_time_counter = 0;
	if (m_body->GetLinearVelocity().y < 0.0f && IsKeyDown(KEY_UP))
	{
		set_velocity_y(m_body->GetLinearVelocity().y - jump_add);
	}

	if (m_body->GetLinearVelocity().y < 0.0f)
	{
		if (AnimationEnded())
		{
			FreezeFrame("P_JUMP", 3);
		}
	}


	else if (m_body->GetLinearVelocity().y >= 0.0f)
	{
		state = PlayerState::Falling;
		SetAnimation("P_FALL");
	}

	
	if (IsKeyDown(KEY_LEFT))
	{
		set_velocity_x(-speed);
		looking_right = false;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		set_velocity_x(speed);
		looking_right = true;
	}
}

void Player::UpdateFallingState(float dt)
{

	if (is_touching_floor)
	{
		coyote_time_counter = coyote_time;
	}
	else
	{
		coyote_time_counter -= dt;
	}

	if (IsKeyPressed(KEY_UP))
	{
		jump_buffer_counter = jump_buffer_time;
	}
	else 
	{
		jump_buffer_counter -= dt;
	}
		
	if(jump_buffer_counter > 0.0f && coyote_time_counter > 0.0f)
	{
		coyote_time_counter = 0.0f;
		jump_buffer_counter = 0.0f;
		set_velocity_y(-jump_force);
		state = PlayerState::Jumping;
		PlayOnce("P_JUMP");
	}

	if (is_touching_floor && m_body->GetLinearVelocity().y > m_ground_slam_vel)
	{
		PlayOnce("P_GROUND");
	}
	else if (is_touching_floor)
	{
		if (AnimationEnded())
		{
			SetAnimation("P_IDLE");
			state = PlayerState::Idle;
		}
	}
	else if (IsKeyDown(KEY_LEFT))
	{
		set_velocity_x(-speed);
		looking_right = false;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		set_velocity_x(speed);
		looking_right = true;
	}

}




