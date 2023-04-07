#include "Player.h"
#include "EntityManager.h"
#include "Settings.h"
#include "Animations.h"
#include <iostream>
#include <box2d.h>
#include "Settings.h"

Player::Player(b2World* physicsWorld)
	:
	physicsWorld(physicsWorld)
{
	SetPos({ 0,0 });
	InitAnimations();
	EnitityManager::Add(this);

	Vector2 level_spawn_position = { (float)GetPos().x / settings::PhysicsWorldScale,
							(float)GetPos().y / settings::PhysicsWorldScale };

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	bodyDef.position.Set(level_spawn_position.x, level_spawn_position.y);

	this->body = physicsWorld->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.9, 1);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 10.0f;

	body->CreateFixture(&fixtureDef);


}

Player::~Player()
{
	EnitityManager::Remove(this);
}


void Player::Update(float dt)
{
	SwitchFrames(dt);
	SetAnimation("BIGZ_IDLE");
	
	if (IsKeyDown(KEY_LEFT))
	{
		pos.x += 1;
		SetAnimation("BIGZ_LEFT");

	}
	if (IsKeyDown(KEY_RIGHT))
	{
		pos.x -= 1;
		SetAnimation("BIGZ_RIGHT");

	}
	if (IsKeyDown(KEY_UP))
	{
		pos.y += 1;
		SetAnimation("BIGZ_UP");

	}
	if (IsKeyDown(KEY_DOWN))
	{
		pos.y -= 1;
		SetAnimation("BIGZ_DOWN");

	}
	
}

void Player::Draw()
{
	auto spritePosX = (body->GetPosition().x * settings::PhysicsWorldScale);
	auto spritePosY = (body->GetPosition().y * settings::PhysicsWorldScale);

	DrawRectangleLines(spritePosX, spritePosY, 16, 16, RED);

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,64,64},
		GetPos(),
		0.0f,
		WHITE);
}

void Player::InitAnimations()
{
	sprite = TextureLoader::GetTexture("BIG_Z");
	animations->InitializeBigZAnimations();
	SetAnimation("BIGZ_IDLE");
}


