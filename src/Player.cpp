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
	StatesStrMap[PlayerState::Attacking] = "Attacking";
	StatesStrMap[PlayerState::Hurting] = "Hurting";
	StatesStrMap[PlayerState::Dying] = "Dying";
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
	feetFixtureName->name = "p_feet";
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
	right_sesnorName->name = "p_r_s";
	left_sesnorName->name = "p_l_s";
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

	//add some mass
	m_fixture->SetDensity(8.0f);
	m_body->ResetMassData();

	//reset some booleans
	is_dying = false;
	//m_has_axe = true;
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

	CheckTouchGround();

	CheckWallTouch();

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
	case PlayerState::Dying:
		UpdateDyingState(dt);
		break;
	case PlayerState::Throwing:
		UpdateThrowingState(dt);
		break;
	case PlayerState::Attacking:
		UpdateAttackingState(dt);
		break;
	case PlayerState::Hurting:
		UpdateHurtingingState(dt);
		break;
	}
}

void Player::Die()
{
	if (!is_dying)
	{
		is_dying = true;
		SetAnimation("P_MELT");
		state = PlayerState::Dying;
	}

}

void Player::SetRandomAttAnim()
{

	int anim_num =  GetRandomValue(0, 1);
	switch (anim_num)
	{
	case 0: SetAnimation("P_ATT1");
		break;
	case 1: SetAnimation("P_ATT2");
		break;
	}
}

void Player::CheckTouchGround()
{
	is_touching_floor = false;
	if (GameScreen::LevelMgr->contacts->player_floor_contacts > 0)
	{
		is_touching_floor = true;
	}

}

void Player::CheckWallTouch()
{
	left_wall_touch = false;
	if (GameScreen::LevelMgr->contacts->player_left_wall_contacts > 0)
	{
		left_wall_touch = true;
	}

	right_wall_touch = false;
	if (GameScreen::LevelMgr->contacts->player_right_wall_contacts > 0)
	{
		right_wall_touch = true;
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

void Player::take_dmg(int dmg)
{
	if (!taking_dmg)
	{
		current_hp -= dmg;
		taking_dmg = true;
	}
}

void Player::Draw(int l)
{

	Rectangle cframe = looking_right ? CurrentFrame() : Rectangle{  CurrentFrame().x,
																	CurrentFrame().y,
																	CurrentFrame().width * -1,
																	CurrentFrame().height};
	auto spritePosX = center_pos().x - 10;
	auto spritePosY = center_pos().y - 12;

	auto axe_spritePosX = center_pos().x +8;
	auto axe_spritePosY = center_pos().y +6;


	if(CurrentFrame().width==96)
	{
		spritePosX = center_pos().x - 43;
		spritePosY = center_pos().y - 44;
	}
	
	if (m_has_axe && !is_dying)
	{
		DrawTexturePro(*axe_sprite,
			{
				10 * 32, 7 * 32,settings::tileSize,settings::tileSize
			},
			Rectangle{ axe_spritePosX,axe_spritePosY,settings::tileSize,settings::tileSize },
			{ settings::tileSize / 2,settings::tileSize / 2 },
			0,
			taking_dmg ? RED : WHITE);
	}

	//BeginShaderMode(shdrOutline);
	DrawTexturePro(*animation->GetTexture(),
		cframe,
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ 0,0 },
		0.0f,
		taking_dmg ? RED : WHITE);

	//EndShaderMode();
	//DrawText(util::VecToString(start_aim_pos).c_str(), center_pos().x, center_pos().y - 10, 10, RED);
	//DrawText(util::VecToString(end_aim_pos).c_str(), center_pos().x, center_pos().y - 20, 10, RED);

}

void Player::DrawUI()
{
	if (is_aiming)
	{
		float y_axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
		float x_axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
		b2Vec2 axe_max = { fabs(x_axis) * (looking_right ? 300.0f : -300.0f) ,
		(looking_right ? (x_axis < 0.0f ? -y_axis*300.0f : y_axis * 300.0f) : (x_axis > 0.0f ? -y_axis * 300.0f : y_axis * 300.0f)) };
		
		//maxpwr
		//for (int i = 0; i < 50; i++) { // three seconds at 60fps
		//	b2Vec2 trajectoryPosition = GetTrajectoryPoint(start_aim_pos, axe_max, i);
		//	DrawRectangleLines(trajectoryPosition.x-1, trajectoryPosition.y-1, 3 + (float(i)* 0.03f), 3 + (float(i) * 0.03f), BLACK);
		//}

		BeginShaderMode(shdrOutline);
		for (int i = 0; i < 50; i++) { // three seconds at 60fps
			b2Vec2 trajectoryPosition = GetTrajectoryPoint(start_aim_pos, axe_velocity, i);
			DrawRectangle(trajectoryPosition.x, trajectoryPosition.y, 1 + (float(i) * 0.03f), 1 + (float(i) * 0.03f), GREEN);
		}
		EndShaderMode();
	}
}

void Player::InitAnimations()
{
	axe_sprite = TextureLoader::GetTexture("MOTHMAN");
	animations->InitializePlayerAnimations();
	SetAnimation("P_IDLE");
}

void Player::UpdateIdleState(float dt)
{
	if ((IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
		&& is_touching_floor)
	{
		set_velocity_y(-jump_force);
		state = PlayerState::Jumping;
		SetAnimation("P_JUMP");
	}
	else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || std::fabs(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X)) > pad_sensitivity_threshold)
	{
		SetAnimation("P_RUN");
		state = PlayerState::Running;
	}

	if (m_body->GetLinearVelocity().y >= 0 && !is_touching_floor)
	{
		state = PlayerState::Falling;
		SetAnimation("P_FALL");
	}

	if (taking_dmg)
	{
		SetAnimation("P_HURT");
		state = PlayerState::Hurting;
	}

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) && m_has_axe)
	{
		SetAnimation("P_AXE_THROW1");
		state = PlayerState::Throwing;
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe)
	{
		SetRandomAttAnim();
		state = PlayerState::Attacking;
	}
}

void Player::UpdateRunningState(float dt)
{
	if ((IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
		&& (is_touching_floor || coyote_time_counter > 0))
	{
		set_velocity_y(-jump_force);
		state = PlayerState::Jumping;
		SetAnimation("P_JUMP");
	}
	else if ((!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) && !IsGamepadAvailable(0)) ||
		fabs(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X)) == 0.0f)
	{
		state = PlayerState::Idle;
		set_velocity_x(0.0f);
		SetAnimation("P_IDLE");
	}
	else if (IsKeyDown(KEY_LEFT) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < -pad_sensitivity_threshold)
	{
		set_velocity_x(-speed);
		looking_right = false;
	}
	else if (IsKeyDown(KEY_RIGHT) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > pad_sensitivity_threshold)
	{
		set_velocity_x(speed);
		looking_right = true;
	}

	if (m_body->GetLinearVelocity().y >= 0 && !is_touching_floor)
	{
		state = PlayerState::Falling;
		SetAnimation("P_FALL");
	}

	if (taking_dmg)
	{
		SetAnimation("P_HURT");
		state = PlayerState::Hurting;
	}

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) && m_has_axe)
	{
		SetAnimation("P_AXE_THROW1");
		state = PlayerState::Throwing;
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe)
	{
		SetRandomAttAnim();
		state = PlayerState::Attacking;
	}



}

void Player::UpdateJumpingState(float dt)
{
	
	coyote_time_counter = 0;
	if (m_body->GetLinearVelocity().y < 0.0f && 
		(IsKeyPressed(KEY_UP) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)))
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

	
	if (IsKeyDown(KEY_LEFT) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < -pad_sensitivity_threshold)
	{
		set_velocity_x(-speed);
		looking_right = false;

	}
	if (IsKeyDown(KEY_LEFT) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > pad_sensitivity_threshold)
	{
		set_velocity_x(speed);
		looking_right = true;
	}

	if (taking_dmg)
	{
		SetAnimation("P_HURT");
		state = PlayerState::Hurting;
	}

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) && m_has_axe)
	{
		SetAnimation("P_AXE_THROW1");
		state = PlayerState::Throwing;
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe)
	{
		SetRandomAttAnim();
		state = PlayerState::Attacking;
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

	if ((IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)))
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
	else if (IsKeyDown(KEY_LEFT) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < -pad_sensitivity_threshold)
	{
		set_velocity_x(-speed);
		looking_right = false;
		
	}
	else if (IsKeyDown(KEY_LEFT) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > pad_sensitivity_threshold)
	{
		set_velocity_x(speed);
		looking_right = true;
		
	}

	if (taking_dmg)
	{
		SetAnimation("P_HURT");
		state = PlayerState::Hurting;
	}

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) && m_has_axe)
	{
		SetAnimation("P_AXE_THROW1");
		state = PlayerState::Throwing;
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe)
	{
		SetRandomAttAnim();
		state = PlayerState::Attacking;
	}



}

void Player::UpdateDyingState(float dt)
{
	if (AnimationEnded())
	{
		is_dying = false;
		taking_dmg = false;
		current_hp = 100;
		state = PlayerState::Idle;
		Vector2 newPos{ 50 ,
						250 };
		GameScreen::camera.target = newPos;
		GameScreen::LevelMgr->new_player_pos = newPos;
		m_body->SetTransform({ newPos.x / settings::PPM, newPos.y / settings::PPM }, 0);
		GameScreen::LevelMgr->next_level = "Level_0";
	}

}


b2Vec2 Player::GetTrajectoryPoint(b2Vec2& startingPosition, b2Vec2& startingVelocity, float n)
{
	//velocity and gravity are given per second but we want time step values here
	float t = 1 / 500.0f; // seconds per time step (at 60fps)
	b2Vec2 stepVelocity = t * startingVelocity; // m/s
	b2Vec2 stepGravity = t * t * world->GetGravity(); // m/s/s

	return startingPosition + n * stepVelocity + 0.5f * (n * n + n) * stepGravity;
}

void Player::UpdateThrowingState(float dt)
{
	if (!axe_anim_thrown)
	{

		is_aiming = true;
		float y_axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
		float x_axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
		float pwr = axe_throw_pwr_counter * 100;

		float x = x_axis * pwr;
		float y = y_axis * pwr;

		start_aim_pos = { center_pos().x + (looking_right ? 18 : m_rectangle.width -20 ), center_pos().y-10};
		axe_velocity = { (looking_right ? fabs(x):-fabs(x)) , (looking_right ? (x_axis<0.0f ? -y : y) : (x_axis > 0.0f ? -y : y)) };
		
	}

	if (animation->GetCurrentFrameNum() == 15 && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))
	{
		PlayFromFrame(12, "P_AXE_THROW1");
	}
	else if(AnimationEnded() || !IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) || (IsKeyUp(KEY_SPACE) && !IsGamepadAvailable(0)))
	{
		if (!axe_anim_thrown)
		{
			is_aiming = false;
			SetAnimation("P_AXE_THROW2");
			axe_anim_thrown = true;
			Rectangle rect = Rectangle{ start_aim_pos.x, start_aim_pos.y , 6, 6 };

			LevelManager::level_entities_safe.push_back(std::make_unique<FireAxe>(rect));
			axe = reinterpret_cast<FireAxe*>(LevelManager::level_entities_safe.back().get());
			axe->m_body->ApplyAngularImpulse(3.5f, true);
			axe->m_body->ApplyLinearImpulseToCenter(axe_velocity, true);
			
			

			if (taking_dmg)
			{
				SetAnimation("P_HURT");
				state = PlayerState::Hurting;
			}
		}
		if (AnimationEnded())
		{
			is_aiming = false;
			SetAnimation("P_IDLE");
			state = PlayerState::Idle;
			axe_anim_thrown = false;
			axe_throw_pwr_counter = 0;
			if (taking_dmg)
			{
				SetAnimation("P_HURT");
				state = PlayerState::Hurting;
			}
		}
	}
	else
	{
		m_has_axe = false;
		if (axe_throw_pwr_counter <= 3.0f)
		{
			axe_throw_pwr_counter += dt*2.0f;
		}
		
		if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
		{
			SetAnimation("P_IDLE");
			state = PlayerState::Idle;
			axe_anim_thrown = false;
			axe_throw_pwr_counter = 0;
			m_has_axe = true;
			if (taking_dmg)
			{
				SetAnimation("P_HURT");
				state = PlayerState::Hurting;
			}
		}
		if (taking_dmg)
		{
			axe_anim_thrown = false;
			axe_throw_pwr_counter = 0;
			m_has_axe = true;
			SetAnimation("P_HURT");
			state = PlayerState::Hurting;
		}
	}
}

void Player::UpdateAttackingState(float dt)
{
	
	if (m_has_axe)
	{
		b2PolygonShape attack_box;
		if (!looking_right)
		{
			attack_box.SetAsBox(1.2, 0.7f, b2Vec2(-1.2, -0.1), 0);
		}
		else
		{
			attack_box.SetAsBox(1.2, 0.7f, b2Vec2(1.2, -0.1), 0);;
		}
		FixtureUserData* attack_sesnorName = new FixtureUserData;
		attack_sesnorName->name = "p_axe_att";
		b2FixtureDef attack_def;
		attack_def.isSensor = false;
		attack_def.shape = &attack_box;
		attack_def.density = 100.0f;
		attack_def.userData.pointer = reinterpret_cast<uintptr_t>(attack_sesnorName);
		m_attack_sensor = m_body->CreateFixture(&attack_def);
		m_body->ResetMassData();
		m_body->SetBullet(true);
		m_has_axe = false;


		m_body->ApplyForce(b2Vec2(looking_right ? -10.0f:10.0f, 0.0f), m_body->GetWorldCenter(), true);
	}

	if (animation->GetCurrentFrameNum() >= 3 && !AnimationEnded() && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) && m_has_axe == false)
	{
		m_has_axe = true;
		m_body->DestroyFixture(m_attack_sensor);
		SetRandomAttAnim();
		state = PlayerState::Attacking;
		if (taking_dmg)
		{
			m_has_axe = true;
			SetAnimation("P_HURT");
			state = PlayerState::Hurting;
		}
	}
	else if (AnimationEnded() && m_has_axe == false)
	{
		m_has_axe = true;
		m_body->DestroyFixture(m_attack_sensor);
		SetAnimation("P_IDLE");
		state = PlayerState::Idle;
		if (taking_dmg)
		{
			m_has_axe = true;
			SetAnimation("P_HURT");
			state = PlayerState::Hurting;
		}
	}

}

void Player::UpdateHurtingingState(float dt)
{
	if (current_hp <= 0)
	{
		Die();
	}
	
	if (!is_dying && AnimationEnded())
	{
		SetAnimation("P_IDLE");
		state = PlayerState::Idle;
		taking_dmg = false;
	}

}




