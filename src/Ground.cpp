#include "Ground.h"
#include "Settings.h"
#include "EntityManager.h"

Ground::Ground()
{
}

Ground::Ground(b2World* physicsWorld)
	:
	physicsWorld(physicsWorld)
{

	EnitityManager::Add(this);
	

	Vector2 level_spawn_position = { (float)GetPos().x / settings::PhysicsWorldScale,
									 (float)GetPos().y / settings::PhysicsWorldScale };
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.fixedRotation = true;
	bodyDef.position.Set(level_spawn_position.x, level_spawn_position.y);

	this->body = physicsWorld->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1, 1);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 10.0f;

	body->CreateFixture(&fixtureDef);
}

Ground::~Ground()
{
}

void Ground::Update(float dt)
{
	
}

void Ground::Draw()
{
	auto spritePosX = (body->GetPosition().x * settings::PhysicsWorldScale) - 16;
	auto spritePosY = (body->GetPosition().y * settings::PhysicsWorldScale) - 16;

	DrawRectangleLines(spritePosX, spritePosY, 32, 32, RED);

}
