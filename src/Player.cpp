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
#include "BioBomb.h"



FireAxe* Player::axe = nullptr;

Player::Player()
	:
	Collidable({ 250,170,12,20 }, b2_dynamicBody, PLAYER) // lvl6
	
{
	NewBody();
	InitAnimations();
	state = PlayerState::Idle;
	m_max_hp = 50;
	current_hp = m_max_hp;

}

void Player::NewBody()
{

	FixtureUserData* data = new FixtureUserData;
	data->tag = PLAYER_GROUP;
	m_fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(data);
	
	//feet collider box
	m_feet_sensor = util::SimpleSensor(m_body, "p_feet", 0.3f, 0.3f, 0, 0.40f);

	//left&right collider boxes
	m_left_sensor = util::SimpleSensor(m_body, "p_l_s", 0.01f, 0.65f, -0.4f, -0.2f);
	m_right_sensor = util::SimpleSensor(m_body, "p_r_s", 0.01f, 0.65f, 0.4f, -0.2f);

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

	// Swtich animation frames for current anim
	SwitchFrames(dt);

	CheckTouchGround();

	CheckWallTouch();

	UpdateSafePos(dt);

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

	invincible_counter += dt;
	if (invincible_counter >= invincible_time)
	{
		invincible = false;
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
	case PlayerState::Sliding:
		UpdateSlidingState(dt);
		break;
	case PlayerState::InDialogue:
		UpdateInDialogueState(dt);
		break;
	case PlayerState::AxeReclaim:
		UpdateAxeReclaimState(dt);
		break;
	}
}

void Player::Die()
{
	if (!is_dying)
	{
		PlaySound(SoundManager::sounds["dead"]);
		is_dying = true;
		SetAnimation("P_MELT");
		state = PlayerState::Dying;
	}

}

void Player::SetAxeAttack()
{

	attack1 = !attack1;
	switch (attack1)
	{
	case 0: 
		SetAnimation("P_ATT1");
		PlaySound(SoundManager::sounds["swish1"]);
		break;
	case 1: 
		SetAnimation("P_ATT2");
		PlaySound(SoundManager::sounds["swish2"]);
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
	if (!taking_dmg && !invincible)
	{
		GameScreen::add_trauma(1.0f);
		invincible = true;
		invincible_counter = 0.0f;
		PlaySound(SoundManager::sounds["grunt"]);
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
	auto spritePosY = center_pos().y - 14;

	auto axe_spritePosX = center_pos().x +8;
	auto axe_spritePosY = center_pos().y +4;


	if(CurrentFrame().width==96)
	{
		spritePosX = center_pos().x - 43;
		spritePosY = center_pos().y - 44;
	}

	Color c = WHITE;
	if (invincible)
	{
		c = int(invincible_counter * 9) % 2 == 0 ? RED : WHITE;
	}
	if (taking_dmg)
	{
		c = RED;
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
		c);

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
		for (int i = 0; i < 50; i++) { // three seconds at 60fps
			b2Vec2 trajectoryPosition = GetTrajectoryPoint(start_aim_pos, axe_max, i);
			DrawRectangleLines(trajectoryPosition.x-1, trajectoryPosition.y-1, 3 + (float(i)* 0.04f), 3 + (float(i) * 0.04f), BLACK);
		}

		//BeginShaderMode(shdrOutline);
		for (int i = 0; i < 50; i++) { // three seconds at 60fps
			b2Vec2 trajectoryPosition = GetTrajectoryPoint(start_aim_pos, axe_velocity, i);
			
			if (i > 46 && (fabs(axe_velocity.x) + fabs(axe_velocity.y)) >= 300.0f)
			{
				DrawRectangle(trajectoryPosition.x, trajectoryPosition.y, 1 + (float(i) * 0.04f), 1 + (float(i) * 0.04f), RED);
			}	
			else if (i > 35 && (fabs(axe_velocity.x) + fabs(axe_velocity.y)) >= 250.0f)
			{
				DrawRectangle(trajectoryPosition.x, trajectoryPosition.y, 1 + (float(i) * 0.04f), 1 + (float(i) * 0.04f), YELLOW);
			}
			else
			{
				DrawRectangle(trajectoryPosition.x, trajectoryPosition.y, 1 + (float(i) * 0.04f), 1 + (float(i) * 0.04f), GREEN);
			}
		}
		//EndShaderMode();
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
		Jump();
	}
	else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || std::fabs(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X)) > pad_sensitivity_threshold)
	{
		SetAnimation("P_RUN");
		state = PlayerState::Running;
		ParticleEmitter* p = new ParticleEmitter({ center_pos().x + 6, center_pos().y + 16 });
		GameScreen::Particles->Add(DefinedEmitter::dust, p);
		p->EmitParticles();
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

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe)
	{
		SetThrowing();
	}
	//axe reclaim
	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && !m_has_axe)
	{
		SetAnimation("P_RECLAIM");
		state = PlayerState::AxeReclaim;
		PlaySound(SoundManager::sounds["creepy_whistle"]);
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe)
	{
		SetAxeAttack();
		state = PlayerState::Attacking;
	}

	//sliding
	if (m_body->GetLinearVelocity().y > 5.0f && m_body->GetLinearVelocity().x != 0.0f && is_touching_floor)
	{
		SetAnimation("P_FALL");
		state = PlayerState::Sliding;
		looking_right = m_body->GetLinearVelocity().x > 0.5f ? true : false;
	}
}

void Player::Jump()
{
	PlaySound(SoundManager::sounds["jump"]);
	set_velocity_y(-jump_force);
	state = PlayerState::Jumping;
	SetAnimation("P_JUMP");
	ParticleEmitter* p = new ParticleEmitter({ center_pos().x+6, center_pos().y + 16 });
	GameScreen::Particles->Add(DefinedEmitter::dust, p);
	p->EmitParticles();
}

void Player::UpdateRunningState(float dt)
{

	//sliding
	if (m_body->GetLinearVelocity().y > 5.0f && m_body->GetLinearVelocity().x != 0.0f && is_touching_floor)
	{
		SetAnimation("P_FALL");
		state = PlayerState::Sliding;
		looking_right = m_body->GetLinearVelocity().x > 0.5f ? true : false;
		ParticleEmitter* p = new ParticleEmitter({ center_pos().x + 6, center_pos().y + 16 });
		GameScreen::Particles->Add(DefinedEmitter::dust, p);
		p->EmitParticles();
	}

	if (!IsSoundPlaying(SoundManager::sounds["step1"]) &&
		!IsSoundPlaying(SoundManager::sounds["step2"]) &&
		!IsSoundPlaying(SoundManager::sounds["step3"]) &&
		!IsSoundPlaying(SoundManager::sounds["step4"]) &&
		!IsSoundPlaying(SoundManager::sounds["step5"]))
	{
		std::string steps[] = { "step1","step2","step3","step4","step5" };
		SoundManager::PlayRandSounds(steps, 5);
	}


	if ((IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
		&& (is_touching_floor || coyote_time_counter > 0))
	{
		Jump();
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

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe)
	{
		SetThrowing();
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe)
	{
		SetAxeAttack();
		state = PlayerState::Attacking;
	}
	//axe reclaim
	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && !m_has_axe)
	{
		SetAnimation("P_RECLAIM");
		state = PlayerState::AxeReclaim;
		PlaySound(SoundManager::sounds["creepy_whistle"]);
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

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe)
	{
		SetThrowing();
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe)
	{
		SetAxeAttack();
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
		PlaySound(SoundManager::sounds["jump"]);
	}

	if (is_touching_floor && m_body->GetLinearVelocity().y > m_ground_slam_vel)
	{
		PlayOnce("P_GROUND");
		PlaySound(SoundManager::sounds["land"]);
		GameScreen::add_trauma(0.3f);
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

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe)
	{
		SetAnimation("P_AXE_THROW1");
		state = PlayerState::Throwing;
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe)
	{
		SetAxeAttack();
		state = PlayerState::Attacking;
	}

}

void Player::UpdateDyingState(float dt)
{
	if (AnimationEnded())
	{
		is_dying = false;
		taking_dmg = false;
		current_hp = m_max_hp;
		state = PlayerState::Idle;
		Vector2 newPos{ 250,170 };
		GameScreen::camera.target = newPos;
		GameScreen::LevelMgr->new_player_pos = newPos;
		m_body->SetTransform({ newPos.x / settings::PPM, newPos.y / settings::PPM }, 0);
		GameScreen::LevelMgr->next_level = "Level_0";
	}

}

void Player::UpdateSlidingState(float dt)
{

	if (taking_dmg)
	{
		SetAnimation("P_HURT");
		state = PlayerState::Hurting;
	}

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe)
	{
		SetThrowing();
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe)
	{
		SetAxeAttack();
		state = PlayerState::Attacking;
	}

	//sliding end
	if (m_body->GetLinearVelocity().y < 5.0f && is_touching_floor)
	{
		SetAnimation("P_IDLE");
		state = PlayerState::Idle;
	}
}

void Player::SetThrowing()
{
	PlaySound(SoundManager::sounds["axe_pickup"]);
	SetAnimation("P_AXE_THROW1");
	state = PlayerState::Throwing;
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

	is_aiming = (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1) && !axe_just_thrown) ? true : false;

	if (is_aiming)
	{
		m_has_axe = false; // pick up axe from back

		//build up power as aiming continues
		if (axe_throw_pwr_counter <= 3.0f)
		{
			axe_throw_pwr_counter += dt * 2.0f;
		}

		//setup throwing vectors
		float y_axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
		float x_axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
		float pwr = axe_throw_pwr_counter * 100;
		float x = x_axis * pwr;
		float y = y_axis * pwr;
		if (x_axis > pad_sensitivity_threshold && !looking_right)
		{
			looking_right = true;
		}
		else if (x_axis < -pad_sensitivity_threshold && looking_right)
		{
			looking_right = false;
		}
		start_aim_pos = { center_pos().x + (looking_right ? 12 : m_rectangle.width - 12), center_pos().y - 6 };
		axe_velocity = { (looking_right ? fabs(x) : -fabs(x)) , (looking_right ? (x_axis < 0.0f ? -y : y) : (x_axis > 0.0f ? -y : y)) };

		//loop animation on aiming
		if (animation->GetCurrentFrameNum() == 15)
		{
			PlayFromFrame(12, "P_AXE_THROW1");
		}

		//if RT pressed - throw!
		if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) || (IsKeyUp(KEY_SPACE) && !IsGamepadAvailable(0)))
		{
			PlaySound(SoundManager::sounds["axe_throw"]);
			axe_just_thrown = true;
			is_aiming = false; // no longer aiming
			SetAnimation("P_AXE_THROW2");
			Rectangle rect = Rectangle{ start_aim_pos.x + (looking_right ? 6 : -8), start_aim_pos.y , 6, 6 };
			LevelManager::level_entities_safe.push_back(std::make_unique<FireAxe>(rect));
			axe = reinterpret_cast<FireAxe*>(LevelManager::level_entities_safe.back().get());
			axe->m_body->ApplyAngularImpulse((looking_right ? 3.5f : -3.5f), true);
			axe->m_body->ApplyLinearImpulseToCenter(axe_velocity, true);
		}

		//if damage taken
		if (taking_dmg)
		{
			SetAnimation("P_HURT");
			state = PlayerState::Hurting;
		}
	}
	else // no logner aiming, axe was thrown or canceled
	{
		if (AnimationEnded("P_AXE_THROW2") && axe_just_thrown) // throwing animation ended
		{
			axe_throw_pwr_counter = 0;
			axe_just_thrown = false;
			m_has_axe = false;
			SetAnimation("P_IDLE");
			state = PlayerState::Idle;
		}
		else if (!axe_just_thrown) // throwing was canceled - give back axe
		{
			axe_throw_pwr_counter = 0;
			axe_just_thrown = false;
			m_has_axe = true;
			SetAnimation("P_IDLE");
			state = PlayerState::Idle;
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
			attack_box.SetAsBox(1.2, 0.6f, b2Vec2(-1.2, -0.2), 0);
		}
		else
		{
			attack_box.SetAsBox(1.2, 0.6f, b2Vec2(1.2, -0.2), 0);;
		}
		FixtureUserData* attack_sesnorName = new FixtureUserData;
		attack_sesnorName->name = "p_axe_att";
		b2FixtureDef attack_def;
		attack_def.isSensor = true;
		attack_def.shape = &attack_box;
		attack_def.density = 100.0f;
		attack_def.userData.pointer = reinterpret_cast<uintptr_t>(attack_sesnorName);
		m_attack_sensor = m_body->CreateFixture(&attack_def);
		m_body->ResetMassData();
		m_has_axe = false;


		m_body->ApplyForce(b2Vec2(looking_right ? -10.0f : 10.0f, 0.0f), m_body->GetWorldCenter(), true);
	}

	if (animation->GetCurrentFrameNum() >= 3 && !AnimationEnded() && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) && m_has_axe == false)
	{
		m_has_axe = true;
		if (!LevelManager::world->IsLocked())
			m_body->DestroyFixture(m_attack_sensor);
		SetAxeAttack();
		state = PlayerState::Attacking;
		if (taking_dmg)
		{
			m_has_axe = true;
			SetAnimation("P_HURT");
			state = PlayerState::Hurting;
		}
	}
	else if (animation->GetCurrentFrameNum() >= 3 && m_has_axe == false)
	{
		m_has_axe = true;
		if (!LevelManager::world->IsLocked())
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
		taking_dmg = false;
		if (is_aiming)
		{
			is_aiming = false;
			m_has_axe = true;
		}
		SetAnimation("P_IDLE");
		state = PlayerState::Idle;
	}

}

void Player::UpdateInDialogueState(float dt)
{
	SetAnimation("P_IDLE");
}

void Player::UpdateAxeReclaimState(float dt)
{
	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)))
	{
		button_pressed_counter += dt;
	}
	else
	{
		button_pressed_counter = 0.0f;
	}
	
	if (!(IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && animation->GetCurrentFrameNum() > 14)
	{
		m_has_axe = true;
		PlayFromFrame(14, "P_RECLAIM");
		PlaySound(SoundManager::sounds["axe_pickup"]);
		//StopSound(SoundManager::sounds["creepy_whistle"]);
		
	}
	else if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && AnimationEnded())
	{
		m_has_axe = true;
		PlaySound(SoundManager::sounds["axe_pickup"]);
		//StopSound(SoundManager::sounds["creepy_whistle"]);
		SetAnimation("P_IDLE");
		if (button_pressed_counter >= 1.71f)
		{
			SetAnimation("P_AXE_THROW1");
			state = PlayerState::Throwing;
			button_pressed_counter = 0.0f;
		}
	}
	else if(!IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
	{
		ParticleEmitter* p = new ParticleEmitter(GameScreen::player->pos());
		GameScreen::Particles->Add(DefinedEmitter::dust, p);
		p->EmitParticles();
		state = PlayerState::Idle;
		SetAnimation("P_IDLE");
		//StopSound(SoundManager::sounds["creepy_whistle"]);
	}
}

void Player::UpdateSafePos(float dt)
{
	safe_pos_counter -= dt;
	if (state != PlayerState::Hurting && !taking_dmg && safe_pos_counter <=0.0f && is_touching_floor && is_standing_on_solid)
	{
		last_safe_pos = pos();
		safe_pos_counter = 3.0f;
	}

	if (!m_body->GetWorld()->IsLocked() && !Vector2Equals(new_pos, old_pos))
	{
		new_pos = old_pos;
		
		m_body->SetTransform(
		{	GameScreen::player->last_safe_pos.x / settings::PPM,
			GameScreen::player->last_safe_pos.y / settings::PPM }, 0);
	}
}




