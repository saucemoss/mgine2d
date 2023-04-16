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
	bodyDef.userData.pointer = (uintptr_t)"SOLID_BLOCK";
	this->body = physicsWorld->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(settings::PhysicsTileUnit, settings::PhysicsTileUnit); //full collider 1sqm = 32px x 32px

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 1.0f;

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
	auto spritePosX = (body->GetPosition().x * settings::PhysicsWorldScale) - settings::tileSize; // -16 to center draw origin
	auto spritePosY = (body->GetPosition().y * settings::PhysicsWorldScale) - settings::tileSize; // -16 to center draw origin
	DrawRectangleLines(spritePosX, spritePosY, settings::drawSize, settings::drawSize, RED);

}
