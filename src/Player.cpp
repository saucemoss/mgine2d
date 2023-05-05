#include "Player.h"
#include "EntityManager.h"
#include "Settings.h"
#include "Animations.h"
#include <iostream>
#include "Settings.h"
#include "GameScreen.h"
#include "Util.h"
#include "SolidTile.h"
#include "raymath.h"
#include <algorithm>



std::string collisionRecText = "";
std::string collisionPointText = "";
std::string collisionResText = "";
std::string sometxt = "";

Player::Player()
{

	rectangle = { 150,150,32,32 };
	x = rectangle.x;
	y = rectangle.y;
	w = rectangle.width;
	h = rectangle.height;
	
	InitAnimations();
	state = PlayerState::Idle;
	m_colliderTag = PLAYER;
	m_colliderDataStr = "player";
	feetSensor = Rectangle{ x,y + h - 4, 32, 8 };
	EnitityManager::Add(this);
	CollisionManager::Add(this);
	

}

Player::~Player()
{
	EnitityManager::Remove(this);
}


void Player::Update(float dt)
{
	// Swtich animation frames for current anim
	SwitchFrames(dt);

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

	// Apply gravity, friction, acceleration
	ApplyForces(dt);

	// Check if level portal
	Rectangle r = { x-10,y,50,32 };
	Collidable* c = CollisionManager::GetCollisionObject(r);
	if (c->m_colliderTag == LEVEL_PORTAL)
	{
		TransformPos(c->m_colliderDataVector2);
		GameScreen::LevelMgr->LoadLevel(c->m_colliderDataStr);
	}

	// Resolve collisions
	CollisionManager::ResolveCollisions(this, dt);

	// Sync colliders and pos
	SyncColliders();
}

void Player::SyncColliders()
{
	rectangle.x = x;
	rectangle.y = y;
	feetSensor.x = x;
	feetSensor.y = y + h - 4;
	pos.x = x;
	pos.y = y;
}


void Player::Draw()
{

	auto spritePosX = x - 16;
	auto spritePosY = y - 32;


	Rectangle cframe = looking_right ? CurrentFrame() : Rectangle{  CurrentFrame().x,
																	CurrentFrame().y,
																	CurrentFrame().width * -1,
																	CurrentFrame().height};
	
	DrawTexturePro(*sprite,
		cframe,
		Rectangle{ spritePosX,spritePosY,settings::drawSize,settings::drawSize },
		{0,0},
		0.0f,
		WHITE);
}

void Player::DrawCollider()
{
	if (colliding) 
	{
		DrawRectangleLinesEx(rectangle, 1, RED);
		
	}
	else 
	{
		DrawRectangleLinesEx(rectangle, 1, GREEN);
		
	}

	//if (CollisionManager::RectSensor(feetSensor))
	//{
	//	DrawRectangleLinesEx(feetSensor, 1, RED);
	//}
	//else
	//{
	//	DrawRectangleLinesEx(feetSensor, 1, GREEN);
	//}


	//DrawText(collisionRecText.c_str(), 5, 80, 20, BLACK);
	//DrawText(("csize: " + std::to_string(CollisionManager::colliders.size())).c_str(), 5, 100, 20, BLACK);

	//DrawText(("ny: " + std::to_string(normals.y)).c_str(), 5, 160, 20, BLACK);
	//DrawText(std::to_string(colliding).c_str(), 5, 180, 20, BLACK);
	//DrawText(("time: " + std::to_string(contactTime)).c_str(), 5, 200, 20, BLACK);
	//Vector2 raydir = GetScreenToWorld2D({ float(GetMousePosition().x),float(GetMousePosition().y) }, GameScreen::camera);

	
	//DrawCircle(contactPoint.x, contactPoint.y, 5, GREEN);
	//DrawLine(contactPoint.x, contactPoint.y, contactPoint.x + normals.x * 10, contactPoint.y+normals.y * 10, YELLOW);
	
}

void Player::InitAnimations()
{
	sprite = TextureLoader::GetTexture("PLAYER");
	animations->InitializePlayerAnimations();
	SetAnimation("P_IDLE");
}

void Player::ApplyForces(float dt)
{
	// Apply gravity
	vy = vy + gravity;

	//Apply friction
	if (vx > 0.0f)
	{
		vx = std::max(0.0f, vx - friction * dt);
	}
	else if (vx < 0.0f)
	{
		vx = std::min(0.0f, vx + friction * dt);
	}
	if (vy > 0.0f)
	{
		vy = std::max(0.0f, vy - friction * dt);
	}
	else if (vy < 0.0f)
	{
		vy = std::min(0.0f, vy + friction * dt);
	}

	// Apply acceleration
	if (vx == 0.0f && IsKeyDown(KEY_LEFT))
	{
		vx = -speed / 2;
	}
	if (vx == 0.0f && IsKeyDown(KEY_RIGHT))
	{
		vx = speed / 2;
	}
	if (vx > 0.0f && IsKeyDown(KEY_RIGHT))
	{
		vx += acceleration * dt;

	}
	if (vx < 0.0f && IsKeyDown(KEY_LEFT))
	{
		vx -= acceleration * dt;

	}
}

void Player::TransformPos(Vector2 pos_in)
{
	x = pos_in.x;
	y = pos_in.y;
	SyncColliders();
}

void Player::UpdateIdleState(float dt)
{
	is_touching_floor = CollisionManager::RectSensor(feetSensor);
	if (IsKeyPressed(KEY_UP) && is_touching_floor)
	{
		vy = -jump_force;
		state = PlayerState::Jumping;
		PlayOnceUninterupt("P_JUMP");
	}
	else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT))
	{
		state = PlayerState::Running;
	}

	if (vy >= 0 && !is_touching_floor)
	{
		state = PlayerState::Falling;
		SetAnimation("P_FALL");
	}
}

void Player::UpdateRunningState(float dt)
{
	is_touching_floor = CollisionManager::RectSensor(feetSensor);
	if (IsKeyPressed(KEY_UP) && (is_touching_floor || coyote_time_counter > 0))
	{
		vy = -jump_force;
		state = PlayerState::Jumping;
		PlayOnceUninterupt("P_JUMP");
	}
	else if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
	{
		state = PlayerState::Idle;
		vx = 0;
		SetAnimation("P_IDLE");
	}
	else if (IsKeyDown(KEY_LEFT))
	{
		state = PlayerState::Running;
		vx = -speed;
		looking_right = false;
		SetAnimation("P_RUN");
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		state = PlayerState::Running;
		vx = speed;
		looking_right = true;
		SetAnimation("P_RUN");
	}

	if (vy >= 0 && !is_touching_floor)
	{
		state = PlayerState::Falling;
		SetAnimation("P_FALL");
	}
}

void Player::UpdateJumpingState(float dt)
{
	coyote_time_counter = 0;
	if (vy < 0.0f && IsKeyDown(KEY_UP))
	{
		vy -= acceleration * 4 * dt;
		if (AnimationEnded())
		{
			FreezeFrame("P_JUMP", 3);
		}
	}
	else if (vy >= 0)
	{
		state = PlayerState::Falling;
		SetAnimation("P_FALL");
	}
	
	if (IsKeyDown(KEY_LEFT))
	{
		vx = -speed;
		looking_right = false;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		vx = speed;
		looking_right = true;
	}
}

void Player::UpdateFallingState(float dt)
{
	is_touching_floor = CollisionManager::RectSensor(feetSensor);

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
		vy = -jump_force;
		state = PlayerState::Jumping;
		PlayOnceUninterupt("P_JUMP");
	}

	if (is_touching_floor && vy > 500)
	{
		PlayOnceUninterupt("P_GROUND");
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
		vx = -speed;
		looking_right = false;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		vx = speed;
		looking_right = true;
	}


}




