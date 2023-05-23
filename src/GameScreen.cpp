#include <raylib.h>
#include <vector>
#include "Util.h"
#include "GameScreen.h"
#include "Player.h"
//#include "EntityManager.h"
#include "Settings.h"
#include "LDtkLoader/Project.hpp"
#include "SolidTile.h"
#include <raymath.h>

std::vector<Entity*> EnitityManager::EntityList;
Camera2D GameScreen::camera;
LevelManager* GameScreen::LevelMgr;
Player* GameScreen::player;
bool GameScreen::debug = false;

GameScreen::GameScreen()

{
	LevelMgr = new LevelManager();
	LevelMgr->LoadLevel("Level_0");

	// Test entities

	player = new Player();

	//Camera init
	camera = { 0 };
	camera.target = {player->x, player->y};
	camera.offset = { settings::screenWidth / 2.0f, settings::screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = settings::zoom;

}

GameScreen::~GameScreen()
{

}

void GameScreen::UpdateCamera(float dt)
{
	// Camera stuff
	// Camera zoom controls
	camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

	if (camera.zoom > 3.0f) camera.zoom = 3.0f;
	else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

	// Camera reset (zoom and rotation)
	if (IsKeyPressed(KEY_R))
	{
		camera.zoom = 1.0f;
		camera.rotation = 0.0f;
	}

	static float minSpeed = 200.0f;
	static float minEffectLength = 20.0f;
	static float fractionSpeed = 6.0f;

	camera.offset = { settings::screenWidth / 2.0f, settings::screenHeight / 2.0f };
	Vector2 diff = Vector2Subtract(player->GetPos(), camera.target);
	float length = Vector2Length(diff);

	if (length > minEffectLength)
	{
		float speed = fmaxf(fractionSpeed * length, minSpeed);
		camera.target = Vector2Add(camera.target, Vector2Scale(diff, speed * dt / length));
	}

	auto levelSize = LevelMgr->currentLdtkLevel->size;
	float minX = 0.0f, minY = 0.0f, maxX = levelSize.x * settings::ScreenScale, maxY = levelSize.y * settings::ScreenScale;
	Vector2 max = GetWorldToScreen2D({ maxX, maxY }, camera);
	Vector2 min = GetWorldToScreen2D({ minX, minY }, camera);
	if (max.x < settings::screenWidth) camera.offset.x = settings::screenWidth - (max.x - settings::screenWidth / 2);
	if (max.y < settings::screenHeight) camera.offset.y = settings::screenHeight - (max.y - settings::screenHeight / 2);
	if (min.x > 0) camera.offset.x = settings::screenWidth / 2 - min.x;
	if (min.y > 0) camera.offset.y = settings::screenHeight / 2 - min.y;


}

Screens GameScreen::Update(float dt)
{
	

	//full screen at current screensize
	if (IsKeyPressed(KEY_F11))
	{
		ToggleFullscreen();
	}

	if (IsKeyPressed(KEY_ONE) && debug == false)
	{
		debug = true;
	}
	else if(IsKeyPressed(KEY_ONE))
	{
		debug = false;
	}

	UpdateCamera(dt);
	LevelMgr->Update(dt);
	player->Update(dt);
	EnitityManager::Update(dt);
	
	return Screens::NONE;
}

void GameScreen::Draw()
{
	auto levelSize = LevelMgr->currentLdtkLevel->size;
	BeginMode2D(camera);
	LevelMgr->Draw();
	EnitityManager::Draw();
	player->Draw();

	//DEBUG:
	if (debug)
	{
		CollisionManager::DrawColliders();
		//player states
		std::string stateStr = "State: " + player->StatesStrMap[player->state];
		DrawText(stateStr.c_str(), player->x, player->y-50, 20, BLACK);
		//player animations
		std::string animStr = "Anim: " + player->animations->m_CurrentActiveAnimation + " :: " +
			std::to_string(player->animations->GetAnimation(player->animations->m_CurrentActiveAnimation)->GetCurrentFrameNum());
		DrawText(animStr.c_str(), player->x, player->y-70, 20, BLACK);
	}
	


	EndMode2D();

	//DEBUG:
	if (debug)
	{
		DrawFPS(5, 5);
		std::string txt = "Entity count: " + std::to_string(EnitityManager::EntityList.size());
		DrawText(txt.c_str(), 5, 20, 20, BLACK);
		if (player != nullptr)
		{
			std::string txt2 = "Player POS: " + VecToString(player->GetPos());
			DrawText(txt2.c_str(), 5, 40, 20, BLACK);
			std::string txt3 = "Is touching floor: " + std::to_string(player->is_touching_floor);
			DrawText(txt3.c_str(), 5, 60, 20, BLACK);

			DrawText(("vx: " + std::to_string(player->vx)).c_str(), 5, 120, 20, BLACK);
			DrawText(("vy: " + std::to_string(player->vy)).c_str(), 5, 140, 20, BLACK);
		}
	}
	
}




