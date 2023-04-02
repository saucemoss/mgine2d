#include <assert.h>
#include "Game.h"
#include "EntityManager.h"
#include <raylib.h>
#include "Settings.h"
#include <iostream>
#include "Animations.h"
#include "BigZombie.h"
#include "ZSpawner.h"

//Player* player = nullptr;
//Player* test = nullptr;
ZSpawner* spawner = nullptr;

Game::Game(int width, int height, int fps, std::string title)
{
	assert(!GetWindowHandle());	//If assertion triggers : Window is already opened
	SetTargetFPS(fps);
	InitWindow(width, height, title.c_str());
	TextureLoader::LoadTextures();
	
	//player = new Player();
	spawner = new ZSpawner();
	//bz = new BigZombie[10];
	//for (int i = 0; i < 10; i++) 
	//{
	//	bz[i].SetPos({ (float)GetRandomValue(-600,0),(float)GetRandomValue(-600,0) });
	//}
}

Game::~Game() noexcept
{
	assert(GetWindowHandle()); //if assertion triggers : Window is already closed
	//delete player;
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
	EnitityManager::Update(dt);
}


void Game::Draw()
{
	ClearBackground(RAYWHITE);
	EnitityManager::Draw();
}

