#include <raylib.h>
#include "GameScreen.h"
#include "Player.h"
#include "ZSpawner.h"
#include "BigZombie.h"
#include "EntityManager.h"

Player* player = nullptr;
ZSpawner* spawner = nullptr;
BigZombie* bz = nullptr;

GameScreen::GameScreen()
{
	player = new Player();
	spawner = new ZSpawner();
	
	bz = new BigZombie[15];
	for (int i = 0; i < 15; i++)
	{
		bz[i].SetPos({ (float)GetRandomValue(-800,0),(float)GetRandomValue(-600,0) });
	}



}

GameScreen::~GameScreen()
{

}

Screens GameScreen::Update(float dt)
{



	EnitityManager::Update(dt);
	return Screens::NONE;
}

void GameScreen::Draw()
{
	EnitityManager::Draw();
	DrawFPS(5, 5);
	std::string txt = "Entity count: " + std::to_string(EnitityManager::EntityList.size());
	DrawText(txt.c_str(), 5, 20, 20, BLACK);
}




