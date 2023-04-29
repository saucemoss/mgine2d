#include "Player.h"
#include "EntityManager.h"
#include "Settings.h"
#include "Animations.h"
#include <iostream>
#include <box2d.h>
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

	m_colliderType = PLAYER;
	m_colliderData = "player";
	feetSensor = Rectangle{ x,y + h - 4,32, 8 };
	EnitityManager::Add(this);
	CollisionManager::Add(this);
	

}

Player::~Player()
{
	EnitityManager::Remove(this);
}


void Player::Update(float dt)
{
	SwitchFrames(dt);
	
	rectangle.x = x;
	rectangle.y = y;
	float speed = 300.0f;
	float acceleration = 1000.0f;
	float friction = 1800.0f;
	float gravity = 60.0f;
	float jump_force = 1100.0f;
	// Apply gravity
	vy = vy + gravity;

	if (IsKeyPressed(KEY_UP) && is_touching_floor)
	{
		vy = -jump_force;
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

	//// Apply friction
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

	//// Apply acceleration
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


	if (vy < 0.0f && IsKeyPressed(KEY_UP) && is_touching_floor)
	{
		vy = -jump_force / 2;
	}
	if (vy < 0.0f && IsKeyDown(KEY_UP))
	{
		vy -= acceleration * 4 * dt;
	}




	is_touching_floor = CollisionManager::RectSensor(feetSensor);
	if (is_touching_floor && vy > gravity)
	{
		PlayOnceUninterupt("P_GROUND");
	}
	else if (!is_touching_floor && vy > 0)
	{
		SetAnimation("P_FALL");
	}
	else if(!is_touching_floor && vy < 0)
	{
		PlayOnce("P_JUMP");
		if (AnimationEnded())
		{
			FreezeFrame("P_JUMP", 3);
		}
	}
	else if (vx != 0)
	{
		SetAnimation("P_RUN");
	}
	else
	{
		SetAnimation("P_IDLE");
	}


	// Resolve collisions
	CollisionManager::ResolveCollisions(this, dt);
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




