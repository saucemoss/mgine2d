#include "Player.h"
#include "EntityManager.h"
#include "Settings.h"
#include "Animations.h"
#include <iostream>
#include <box2d.h>
#include "Settings.h"
#include "GameScreen.h"
#include "Util.h"


Player::Player(b2World* physicsWorld)
	:
	physicsWorld(physicsWorld)
{
	SetPos({ 150,150 });
	InitAnimations();
	EnitityManager::Add(this);

	//
	Vector2 collider_pos_center = { ((float)GetPos().x )  / settings::PhysicsWorldScale,
									((float)GetPos().y )  / settings::PhysicsWorldScale };

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	bodyDef.position.Set(collider_pos_center.x, collider_pos_center.y);

	this->body = physicsWorld->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(settings::PhysicsTileUnit /2, settings::PhysicsTileUnit / 2); // set only half of unit (1m=32px) of collider = (16px x 16px)


	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 1.0f;

	body->CreateFixture(&fixtureDef);


}

Player::~Player()
{
	EnitityManager::Remove(this);
}


void Player::Update(float dt)
{
	UpdatePosition();
	SwitchFrames(dt);
	SetAnimation("P_IDLE");
	PlayerMovementControl(dt);

}

void Player::UpdatePosition()
{
	SetPos({ body->GetPosition().x * settings::PhysicsWorldScale, body->GetPosition().y * settings::PhysicsWorldScale });
}

void Player::PlayerMovementControl(float dt)
{
	// dampen horizontal movement
	const float horizontalDampeningFactor = 1;
	set_velocity_x(body->GetLinearVelocity().x * (1 - dt * horizontalDampeningFactor));

	check_if_on_floor();
	check_if_move();
	check_if_jump();
}

void Player::set_velocity_x(float vx)
{
	body->SetLinearVelocity({
		vx,
		body->GetLinearVelocity().y,
		});
}

void Player::set_velocity_y(float vy)
{
	body->SetLinearVelocity({
		body->GetLinearVelocity().x,
		vy,
		});
}

void Player::set_velocity_xy(float vx, float vy)
{
	body->SetLinearVelocity({ vx, vy });
}

void Player::check_if_on_floor()
{
	// first, reset whether we're touching floor
	is_touching_floor = false;

	// check left, center, and right touch points
	float x_deviations[] = { -2.0f, 0.0f, 2.0f };

	for (auto x_dev : x_deviations)
	{
		// query raylib to see if we're touching floor
		auto source = body->GetPosition();
		source.x += x_dev;

		auto target = body->GetPosition();
		target.x += x_dev;
		target.y += 1.1;
		
		is_touching_floor = RaycastCheckCollisionWithUserData(
			physicsWorld,
			source,
			target,
			"SOLID_BLOCK");

		if (is_touching_floor)
		{
			break;
		}
	}
}

bool Player::can_move_in_x_direction(bool moving_right)
{
	float y_deviations[] = { -1.0f, 0.0f, 1.0f };
	for (auto y_dev : y_deviations)
	{
		// query raylib to see if we're touching floor
		auto source = body->GetPosition();
		source.y += y_dev;

		auto target = body->GetPosition();
		target.y += y_dev;

		// check left side if necessary
		target.x += (moving_right ? 1 : -1) * 1.1;

		//auto is_agains_wall = RaycastCheckCollisionWithUserData(
		//	GameScene::world,
		//	source,
		//	target,
		//	PhysicsTypes::SolidBlock);

		//if (is_agains_wall)
		//{
		//	return false;
		//}
	}

	return true;
}

void Player::check_if_jump()
{
	if (is_touching_floor && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE)))
	{
		set_velocity_y(-25);
	}

	if (abs(body->GetLinearVelocity().x) > 0)
	{
		SetAnimation("P_RUN");
	}

	if (!is_touching_floor)
	{
		auto vel = body->GetLinearVelocity().y;
		const int jump_threshold = 5;

		if (vel > jump_threshold)
		{
			SetAnimation("P_FALL");
		}
		else if (vel < -jump_threshold)
		{
			SetAnimation("P_JUMP");
		}
		else
		{
			FreezeFrame("P_JUMP", 3);
		}
	}
}

void Player::check_if_move()
{
	const auto effective_speed = 15.0f;
	if (IsKeyDown(KEY_LEFT) && can_move_in_x_direction(false))
	{
		looking_right = false;
		set_velocity_x(-effective_speed);
	}

	if (IsKeyDown(KEY_RIGHT) && can_move_in_x_direction(true))
	{
		looking_right = true;
		set_velocity_x(effective_speed);
	}
}


void Player::Draw()
{
	auto spritePosX = (body->GetPosition().x* settings::PhysicsWorldScale) - 32;
	auto spritePosY = (body->GetPosition().y* settings::PhysicsWorldScale) - 48;

	auto colliderPosX = (body->GetPosition().x * settings::PhysicsWorldScale);
	auto colliderPosY = (body->GetPosition().y * settings::PhysicsWorldScale);
	DrawRectangleLines(colliderPosX-16, colliderPosY-16 , settings::tileSize, settings::tileSize, RED);



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

void Player::InitAnimations()
{
	sprite = TextureLoader::GetTexture("PLAYER");
	animations->InitializePlayerAnimations();
	SetAnimation("P_IDLE");
}




