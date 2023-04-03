#include <assert.h>
#include "Game.h"
#include "EntityManager.h"
#include <raylib.h>
#include "Settings.h"
#include <iostream>
#include "Animations.h"
#include "BigZombie.h"
#include "ZSpawner.h"

Player* player = nullptr;
BigZombie* bz = nullptr;
ZSpawner* spawner = nullptr;

Game::Game(int width, int height, int fps, std::string title)
{
	assert(!GetWindowHandle());	//If assertion triggers : Window is already opened
	SetTargetFPS(fps);
	InitWindow(width, height, title.c_str());
	TextureLoader::LoadTextures();
	
	player = new Player();
	spawner = new ZSpawner();
	bz = new BigZombie[15];
	for (int i = 0; i < 15; i++) 
	{
		bz[i].SetPos({ (float)GetRandomValue(-800,0),(float)GetRandomValue(-600,0) });
	}
}

Game::~Game() noexcept
{
	assert(GetWindowHandle()); //if assertion triggers : Window is already closed
	delete player;
	delete[] bz;
	CloseWindow();
}

bool Game::GameShouldClose() const
{
	return WindowShouldClose();
}

void Game::Tick()
{
	BeginDrawing();
	Update();
	Draw();
	EndDrawing();
}

void Game::Update()
{
	float dt = GetFrameTime();

	if (IsKeyPressed(KEY_ONE))
	{
		BigZombie* bigz = new BigZombie();
		bigz->SetPos({ (float)GetRandomValue(-800,0),(float)GetRandomValue(-600,0) });
	}

	if (IsKeyPressed(KEY_R))
	{
		delete[] bz;
	}

	EnitityManager::Update(dt);
}


void Game::Draw()
{
	ClearBackground(RAYWHITE);
	
	EnitityManager::Draw();
	DrawFPS(5, 5);
	std::string txt = "Entity count: " + std::to_string(EnitityManager::EntityList.size());
	DrawText(txt.c_str(), 5, 20, 20, BLACK);
}

