#include "nlohmann/json.hpp"
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
#include <random>


using json = nlohmann::json;
FireAxe* Player::axe = nullptr;

Player::Player(Vector2 pos)
	:
	Collidable({ pos.x,pos.y,12,20 }, b2_dynamicBody, PLAYER)
	
{
	NewBody();
	InitAnimations();
	LoadAbilities();
	state = PlayerState::Idle;
	m_max_hp = 50;
	current_hp = m_max_hp;
	m_max_energy = 50;
	current_energy = m_max_energy;
	energy_regen_counter = energy_regen_interval;
	wall_jump_counter = wall_jump_time;
	wall_jump_hang_counter = wall_jump_hang_time;

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

	//left&right collider boxes
	m_r_grab_sensor = util::SimpleSensor(m_body, "p_lg_s", 0.4f, 0.4f, -0.4f, -0.2f);
	m_l_grab_sensor = util::SimpleSensor(m_body, "p_rg_s", 0.4f, 0.4f, 0.4f, -0.2f);

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

	RegenEnergy(dt);

	GenerateOrbs(dt);

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
	case PlayerState::MediPodSave:
		UpdateMediPodSaveState(dt);
		break;
	case PlayerState::Dashing:
		UpdateDashingState(dt);
		break;
	case PlayerState::PwrAttacking:
		UpdatePowerAttackingState(dt);
		break;
	case PlayerState::WallGrabbing:
		UpdateWallGrabbingState(dt);
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

void Player::KnockBack(Vector2 direction)
{

	m_body->ApplyLinearImpulseToCenter(b2Vec2{ direction.x, direction.y }, true);
}

void Player::LoadAbilities()
{
	std::ifstream loadFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
	json save_data;
	if (loadFile.is_open())
	{
		loadFile >> save_data;
		loadFile.close();
	}


	m_axe_unlocked = save_data["player"]["skillsUnlocked"]["axe"];
	m_has_axe = save_data["player"]["skillsUnlocked"]["axe"];
	m_throwing_unlocked = save_data["player"]["skillsUnlocked"]["axe_throwing"];
	m_power_attack_unlocked = save_data["player"]["skillsUnlocked"]["axe_pwr_attack"];
	m_axe_dash_unlocked = save_data["player"]["skillsUnlocked"]["axe_dash_slash"];

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


	left_grab = false;
	if (GameScreen::LevelMgr->contacts->player_left_grab_contacts > 0)
	{
		left_grab = true;
	}

	right_grab = false;
	if (GameScreen::LevelMgr->contacts->player_right_grab_contacts > 0)
	{
		right_grab = true;
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
	if (!taking_dmg && !invincible && !is_dashing)
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
	if (!visible)
	{
		return;
	}

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
	if (is_dashing)
	{
		//c = Fade(c, 0.9f);
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

bool Player::WithdrawEnergy(int energy_to_withdraw)
{

	if (energy_to_withdraw <= current_energy)
	{
		current_energy -= energy_to_withdraw;
		return true;
	}
	else
	{
		return false;
	}

}

void Player::RegenEnergy(float dt)
{
	if (!m_energy_unlocked)
	{
		return;
	}

	energy_regen_counter -= dt;
	if (energy_regen_counter <= 0.0f && current_energy < m_max_energy)
	{
		current_energy++;
		energy_regen_counter = energy_regen_interval;
	}
}

void Player::GenerateOrbs(float dt)
{
	if (!m_energy_unlocked)
	{
		return;
	}

	spawn_counter -= dt;
	if (spawn_counter <= 0.0f)
	{
		// Create a random number generator
		std::random_device rd;
		std::mt19937 gen(rd());

		// Define a distribution for generating random positions within the radius
		float m_spawn_radius = 64.0f;
		std::uniform_real_distribution<float> distribution(-m_spawn_radius, m_spawn_radius);

		// Generate random positions
		float posX = distribution(gen) + pos().x;
		float posY = distribution(gen) + pos().y;

		GameScreen::LevelMgr->queue_entities.push_back({ "EOrb", {posX, posY} });
		spawn_counter = spawn_interwal;
	}
}

void Player::UpdateIdleState(float dt)
{
	if ((IsKeyPressed(KEY_LEFT_SHIFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
		&& m_axe_dash_unlocked && m_has_axe && current_energy >= dash_cost)
	{
		SetDashing();
		return;
	}
	else if ((IsMouseButtonPressed(2) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))
		&& m_power_attack_unlocked && m_has_axe && current_energy >= pwr_att_cost)
	{
		SetPwrAttack();
		return;
	}
	
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

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe && m_throwing_unlocked)
	{
		SetThrowing();
	}
	//axe reclaim
	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && !m_has_axe && m_throwing_unlocked)
	{
		SetAnimation("P_RECLAIM");
		state = PlayerState::AxeReclaim;
		PlaySound(SoundManager::sounds["creepy_whistle"]);
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe && m_axe_unlocked)
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
	if ((IsKeyPressed(KEY_LEFT_SHIFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
		&& m_axe_dash_unlocked && m_has_axe && current_energy >= dash_cost)
	{
		SetDashing();
		return;
	}
	else if ((IsMouseButtonPressed(2) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))
		&& m_power_attack_unlocked && m_has_axe && current_energy >= pwr_att_cost)
	{
		SetPwrAttack();
		return;
	}

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

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe && m_throwing_unlocked)
	{
		SetThrowing();
	}
	//axe reclaim
	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && !m_has_axe && m_throwing_unlocked)
	{
		SetAnimation("P_RECLAIM");
		state = PlayerState::AxeReclaim;
		PlaySound(SoundManager::sounds["creepy_whistle"]);
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe && m_axe_unlocked)
	{
		SetAxeAttack();
		state = PlayerState::Attacking;
	}

}

void Player::UpdateJumpingState(float dt)
{

	coyote_time_counter = 0;
	if ((IsKeyPressed(KEY_LEFT_SHIFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
		&& m_axe_dash_unlocked && m_has_axe && current_energy >= dash_cost)
	{
		SetDashing();
		return;
	}
	else if ((IsMouseButtonPressed(2) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))
		&& m_power_attack_unlocked && m_has_axe && current_energy >= pwr_att_cost)
	{
		SetPwrAttack();
		return;
	}
	
	if (m_body->GetLinearVelocity().y < 0.0f && 
		(IsKeyPressed(KEY_UP) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)))
	{
		set_velocity_y(m_body->GetLinearVelocity().y - jump_add * dt);
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

	//if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe && m_throwing_unlocked)
	//{
	//	SetThrowing();
	//}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe && m_axe_unlocked)
	{
		SetAxeAttack();
		state = PlayerState::Attacking;
	}


	if ((left_grab || right_grab) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) && m_has_axe && !is_touching_floor)
	{
		SetWallGrab();
	}


}

void Player::UpdateFallingState(float dt)
{
	if ((IsKeyPressed(KEY_LEFT_SHIFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
		&& m_axe_dash_unlocked && m_has_axe && current_energy >= dash_cost)
	{
		SetDashing();
		return;
	}
	else if ((IsMouseButtonPressed(2) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))
		&& m_power_attack_unlocked && m_has_axe && current_energy >= pwr_att_cost)
	{
		SetPwrAttack();
		return;
	}

	if ((left_grab || right_grab) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) && m_has_axe && !is_touching_floor)
	{
		SetWallGrab();
	}

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

	//if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe && m_throwing_unlocked)
	//{
	//	SetThrowing();
	//}
	//axe reclaim
	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && !m_has_axe && m_throwing_unlocked)
	{
		SetAnimation("P_RECLAIM");
		state = PlayerState::AxeReclaim;
		PlaySound(SoundManager::sounds["creepy_whistle"]);
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe && m_axe_unlocked)
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

		std::ifstream loadFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
		json save_data;
		if (loadFile.is_open())
		{
			loadFile >> save_data;
			loadFile.close();
		}

		// Update the current level and player position
		float posX = save_data["player"]["position"][0];
		float posY = save_data["player"]["position"][1];
		Vector2 new_player_pos = { posX - 16.0f, posY - 16.0f };

		
		GameScreen::camera.target = new_player_pos;
		GameScreen::LevelMgr->new_player_pos = new_player_pos;
		m_body->SetTransform({ new_player_pos.x / settings::PPM, new_player_pos.y / settings::PPM }, 0);
		GameScreen::LevelMgr->next_level = save_data["player"]["currentLevel"];
	}

}

void Player::UpdateSlidingState(float dt)
{
	if ((IsKeyPressed(KEY_LEFT_SHIFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
		&& m_axe_dash_unlocked && m_has_axe && current_energy >= dash_cost)
	{
		SetDashing();
		return;
	}
	else if ((IsMouseButtonPressed(2) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))
		&& m_power_attack_unlocked && m_has_axe && current_energy >= pwr_att_cost)
	{
		SetPwrAttack();
		return;
	}

	if (taking_dmg)
	{
		SetAnimation("P_HURT");
		state = PlayerState::Hurting;
	}

	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && m_has_axe && m_throwing_unlocked)
	{
		SetThrowing();
	}
	//axe reclaim
	if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && !m_has_axe && m_throwing_unlocked)
	{
		SetAnimation("P_RECLAIM");
		state = PlayerState::AxeReclaim;
		PlaySound(SoundManager::sounds["creepy_whistle"]);
	}

	if ((IsKeyDown(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) && m_has_axe && m_axe_unlocked)
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
	if (!m_throwing_unlocked)
	{
		return;
	}
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

void Player::SetPwrAttack()
{
	StopSound(SoundManager::sounds["steam_thud"]);
	StopSound(SoundManager::sounds["axe_solid_hit"]);
	if (WithdrawEnergy(pwr_att_cost))
	{
		PlaySound(SoundManager::sounds["steam"]);
		SetAnimation("PWR_ATT");
		state = PlayerState::PwrAttacking;
		m_has_axe = false;
		axe_dmg += 200;
	}

}



void Player::UpdatePowerAttackingState(float dt)
{
	m_body->SetLinearVelocity({ 0.0f,0.0f });

	if (animation->GetCurrentFrameNum() < 7)
	{
		m_body->ApplyForce(b2Vec2(0.0f, -world->GetGravity().y * m_body->GetMass()), m_body->GetWorldCenter(), true);
	}

	if (animation->GetCurrentFrameNum() >= 8 && !is_touching_floor)
	{
		m_body->SetLinearVelocity({ 0.0f,30.0f });
	}

	if (animation->GetCurrentFrameNum() >= 8 && !pwr_sensor_on)
	{
		PlaySound(SoundManager::sounds["swish2"]);
		pwr_att = util::SimpleSensor(m_body, "p_axe_att", 1.5f, 1.3f, (looking_right ? 1.5 : -1.5f), -0.8f);
		m_body->ApplyForce(b2Vec2(looking_right ? -10.0f : 10.0f, 0.0f), m_body->GetWorldCenter(), true);
		pwr_sensor_on = true;
	}
	if (animation->GetCurrentFrameNum() == 9)
	{
		for (int i = 0; i < 5; i++)
		{

			ParticleEmitter* p = new ParticleEmitter({ pos().x + (looking_right ? 36.0f + i * 2 : -36.0f - i * 2), pos().y + i });
			GameScreen::Particles->Add(DefinedEmitter::dust, p);
			p->EmitParticles();
		}

		GameScreen::add_trauma(1.0f);

		if (!IsSoundPlaying(SoundManager::sounds["steam_thud"]))PlaySound(SoundManager::sounds["steam_thud"]);
		if (!IsSoundPlaying(SoundManager::sounds["axe_solid_hit"]))PlaySound(SoundManager::sounds["axe_solid_hit"]);
	}

	if (AnimationEnded())
	{
		if (!LevelManager::world->IsLocked())
		{
			m_body->DestroyFixture(pwr_att);
			pwr_sensor_on = false;
		}

		state = PlayerState::Idle;
		SetAnimation("P_IDLE");
		m_has_axe = true;
		axe_dmg = 35;
	}
}

void Player::UpdateHurtingingState(float dt)
{

	if (current_hp <= 0)
	{
		Die();
	}

	if (!is_dying && (IsKeyPressed(KEY_LEFT_SHIFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
		&& m_axe_dash_unlocked && m_has_axe && current_energy >= dash_cost)
	{
		SetDashing();
		taking_dmg = false;
		if (is_aiming)
		{
			is_aiming = false;
			m_has_axe = true;
		}
		return;
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


	if (current_energy < axe_spawn_cost) // not enough energy - go to idle
	{
		ParticleEmitter* p = new ParticleEmitter(GameScreen::player->pos());
		GameScreen::Particles->Add(DefinedEmitter::dust, p);
		p->EmitParticles();
		PlaySound(SoundManager::sounds["steam"]);
		state = PlayerState::Idle;
		SetAnimation("P_IDLE");
	}
	
	// continue autofinishing spawning after frame 14
	if (finish_axe_reclaim_anim)
	{
		if (AnimationEnded())
		{
			m_has_axe = true;
			PlaySound(SoundManager::sounds["axe_pickup"]);
			WithdrawEnergy(axe_spawn_cost);
			SetAnimation("P_IDLE");
			state = PlayerState::Idle;
			button_pressed_counter = 0.0f;
			finish_axe_reclaim_anim = false;
		}
	}
	// after frame 14 set autofinish spawning if button is not pressed
	else if (!(IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && animation->GetCurrentFrameNum() >= 14 && !AnimationEnded()) 
	{
		finish_axe_reclaim_anim = true;
		PlayFromFrame(animation->GetCurrentFrameNum(), "P_RECLAIM");
	}
	else if ((IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) && AnimationEnded())
	{
		if(!m_has_axe) WithdrawEnergy(axe_spawn_cost);
		m_has_axe = true;
		PlaySound(SoundManager::sounds["axe_pickup"]);
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
		ParticleEmitter* p = new ParticleEmitter(pos());
		GameScreen::Particles->Add(DefinedEmitter::dust, p);
		p->EmitParticles();
		state = PlayerState::Idle;
		SetAnimation("P_IDLE");

	}
}

void Player::UpdateMediPodSaveState(float dt)
{
	visible = false;
}

void Player::SetDashing()
{
	is_dashing = true;
	for (int i = 0; i < 4; i++)
	{
		Vector2 ppos = { GameScreen::player->pos().x + (looking_right ? -6 : 6), GameScreen::player->pos().y - i * 4};
		ParticleEmitter* p1 = new ParticleEmitter(ppos);
		GameScreen::Particles->Add(DefinedEmitter::dust, p1);
		p1->EmitParticles();
	}

	b2ContactEdge* edge = m_body->GetContactList();
	while (edge != NULL)
	{
		auto c = reinterpret_cast<Collidable*>(edge->other->GetUserData().pointer);
		if (c->m_colliderTag != SOLID)
		{
			edge->contact->SetEnabled(false);
		}
		edge = edge->next;
	}

	WithdrawEnergy(dash_cost);
	PlaySound(SoundManager::sounds["steam"]);
	SetAnimation("P_DASH");
	state = PlayerState::Dashing;
	m_has_axe = false;
	axe_dmg += 200;

	dash_att = util::SimpleSensor(m_body, "p_axe_att", 1.5f, 0.6f);
	m_body->ApplyForce(b2Vec2(looking_right ? -10.0f : 10.0f, 0.0f), m_body->GetWorldCenter(), true);
	m_has_axe = false;

}



void Player::UpdateDashingState(float dt)
{
	m_body->SetLinearVelocity({ (looking_right ? speed * 4 : speed * 4 * -1), 0.0f });
	m_body->ApplyForce(b2Vec2(0.0f, -world->GetGravity().y * m_body->GetMass()), m_body->GetWorldCenter(), true);
	dashing_counter += dt;
	ParticleEmitter* p = new ParticleEmitter({ center_pos().x + (looking_right ? - 4 : 4), center_pos().y + 4});
	GameScreen::Particles->Add((looking_right ? DefinedEmitter::dash_shadow : DefinedEmitter::dash_shadow_left), p);
	p->EmitParticles();

	if (dashing_counter >= dashing_time)
	{
		axe_dmg = 35;
		is_dashing = false;
		m_has_axe = true;
		dashing_counter = 0.0f;
		m_body->SetLinearVelocity({ 0.0f, 0.0f });
		state = PlayerState::Idle;
		SetAnimation("P_IDLE");
		ParticleEmitter* p = new ParticleEmitter(pos());
		GameScreen::Particles->Add(DefinedEmitter::dust, p);
		p->EmitParticles();
		if (!LevelManager::world->IsLocked()) m_body->DestroyFixture(dash_att);
	}
}

void Player::SetWallGrab()
{
	SetAnimation("P_W_GRB");
	state = PlayerState::WallGrabbing;
	m_has_axe = false;
	ParticleEmitter* p = new ParticleEmitter({ center_pos().x + (looking_right ? + 16 : - 6), center_pos().y });
	GameScreen::Particles->Add(DefinedEmitter::dust, p);
	p->EmitParticles();
	PlaySound(SoundManager::sounds["axe_solid_hit"]);
}

void Player::UpdateWallGrabbingState(float dt)
{
	

	if (!left_grab && !right_grab)
	{
		wall_let_go = false;
		wall_jump_counter = wall_jump_time;
		wall_jump_hang_counter = wall_jump_hang_time;
		state = PlayerState::Falling;
		SetAnimation("P_FALL");
		m_has_axe = true;
	}

	if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
	{
		wall_jump_hang_counter -= dt;
		if (wall_jump_hang_counter >= 0.0f)
		{
			m_body->ApplyForce(b2Vec2(0.0f, -world->GetGravity().y * m_body->GetMass()), m_body->GetWorldCenter(), true);
		}
		else if ((int)(wall_jump_hang_counter*10) % 5 == 0)
		{
			ParticleEmitter* p = new ParticleEmitter({ center_pos().x + (looking_right ? +16 : -6), center_pos().y });
			GameScreen::Particles->Add(DefinedEmitter::dust, p);
			p->EmitParticles();
		}

		m_body->SetLinearVelocity({ 0.0f, 0.0f});
	}
	else
	{
		wall_let_go = true;
		wall_jump_counter -= dt;
		if (wall_jump_counter <= 0.0f)
		{
			state = PlayerState::Falling;
			SetAnimation("P_FALL");
			wall_let_go = false;
			wall_jump_counter = wall_jump_time;
			wall_jump_hang_counter = wall_jump_hang_time;
			m_has_axe = true;
		}
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) && wall_let_go)
	{
		wall_let_go = false;
		wall_jump_counter = wall_jump_time;
		wall_jump_hang_counter = wall_jump_hang_time;
		m_has_axe = true;
		m_body->ApplyForceToCenter({ (looking_right ? 170.0f : -170.0f), 0.0f }, true);
		ParticleEmitter* p = new ParticleEmitter({ center_pos().x + (looking_right ? +12 : -12), center_pos().y });
		GameScreen::Particles->Add(DefinedEmitter::dust, p);
		p->EmitParticles();
		Jump();

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




