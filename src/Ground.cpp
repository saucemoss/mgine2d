#include "Ground.h"
#include "Settings.h"
#include "EntityManager.h"

Ground::Ground(b2World* physicsWorld)
	:
	physicsWorld(physicsWorld)
{
	EnitityManager::Add(this);
	this->SetPos(0, 600);

	Vector2 level_spawn_position = { (float)pos.x / settings::PhysicsWorldScale,
						(float)pos.y / settings::PhysicsWorldScale };
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.fixedRotation = true;
	bodyDef.position.Set(level_spawn_position.x, level_spawn_position.y);

	this->body = physicsWorld->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(50, 50);

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
	auto spritePosX = (body->GetPosition().x * settings::PhysicsWorldScale);
	auto spritePosY = (body->GetPosition().y * settings::PhysicsWorldScale);

	DrawRectangle(spritePosX, spritePosY, 64, 64, RED);

}
