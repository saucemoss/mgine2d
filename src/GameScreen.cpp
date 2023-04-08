#include <raylib.h>
#include "GameScreen.h"
#include "Player.h"
#include "ZSpawner.h"
#include "BigZombie.h"
#include "EntityManager.h"
#include "Ground.h"
#include "Util.h"
#include "Settings.h"

Player* player = nullptr;
ZSpawner* spawner = nullptr;
BigZombie* bz = nullptr;
b2World* GameScreen::b2world = nullptr;

GameScreen::GameScreen()
{
	if (b2world != nullptr)
	{
		// if we had an old world then delete it and recreate
		// a new one for the new level
		delete b2world;
		b2world = nullptr;
	}
	b2Vec2 gravity(0.0f, 60.0f);
	b2world = new b2World(gravity);


	spawner = new ZSpawner();
	
	Ground* ground = new Ground[8];

	for (int i = 0; i < 8; i++)
	{
		ground[i] = *(new Ground(b2world));


		ground[i].body->SetTransform({ (float)((50 + i * 32) / settings::PhysicsWorldScale), 
									(float)300 / settings::PhysicsWorldScale }, 0);
	}

	Ground* ground2 = new Ground(b2world);

	ground2->body->SetTransform({ (float)114 / settings::PhysicsWorldScale,
									(float)268 / settings::PhysicsWorldScale }, 0);
	


}

GameScreen::~GameScreen()
{

}

Screens GameScreen::Update(float dt)
{

	// Box2d 
	const float timeStep = 1.0f / 60.0f;
	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;
	b2world->Step(timeStep, velocityIterations, positionIterations);
	EnitityManager::Update(dt);

	if (IsKeyPressed(KEY_X))
	{
		player = new Player(b2world);
	}

	
	
	return Screens::NONE;
}

void GameScreen::Draw()
{
	EnitityManager::Draw();
	DrawFPS(5, 5);
	std::string txt = "Entity count: " + std::to_string(EnitityManager::EntityList.size());
	DrawText(txt.c_str(), 5, 20, 20, BLACK);
	if (player != nullptr)
	{
		std::string txt2 = "Player POS: " + VecToString(player->GetPos());
		DrawText(txt2.c_str(), 5, 40, 20, BLACK);
	}

}




