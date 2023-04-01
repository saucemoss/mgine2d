#include <assert.h>
#include "Game.h"
#include "EntityManager.h"
#include <raylib.h>
#include "Settings.h"
#include <iostream>
#include "Animations.h"
#include "BigZombie.h"

Player* player = nullptr;
BigZombie* bz[3];

Game::Game(int width, int height, int fps, std::string title)
{
	assert(!GetWindowHandle());	//If assertion triggers : Window is already opened
	SetTargetFPS(fps);
	InitWindow(width, height, title.c_str());
	
	player = new Player();

	for (int i = 0; i < 3; i++) 
	{
		bz[i] = new BigZombie();
		bz[i]->SetPos({ (float)i * 10,(float)i * 10 });
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
	EnitityManager::Update(dt);
}


void Game::Draw()
{
	ClearBackground(RAYWHITE);
	player->draw();
	for (int i = 0; i < 3; i++)
	{
		bz[i]->draw();
	}
}

