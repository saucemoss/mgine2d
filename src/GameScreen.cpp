#include <raylib.h>
#include <vector>
#include "Util.h"
#include "GameScreen.h"
#include "Settings.h"
#include "LDtkLoader/Project.hpp"
#include <raymath.h>
#include "rlgl.h"

std::vector<Entity*> EnitityManager::EntityList;
Camera2D GameScreen::camera;
Camera2D GameScreen::player_focused_cam;
LevelManager* GameScreen::LevelMgr;
Player* GameScreen::player;
bool GameScreen::debug = false;


GameScreen::GameScreen()

{
	HideCursor();
	
	LevelMgr = new LevelManager();

	player = new Player();
	
	//Camera init

	player_focused_cam.target = player->pos();
	player_focused_cam.offset = { settings::screenWidth / 2.0f, settings::screenHeight / 2.0f };
	player_focused_cam.zoom = settings::zoom;
	camera = { 0 };
	camera.target = player->pos();
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
	// 
	// Camera zoom controls
	player_focused_cam.target = player->pos();
	camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

	if (camera.zoom > 3.0f) camera.zoom = 3.0f;
	else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

	//// Camera reset (zoom and rotation)
	//if (IsKeyPressed(KEY_R))
	//{
	//	camera.zoom = 1.0f;
	//	camera.rotation = 0.0f;
	//}

	static float minSpeed = 1.0f;
	static float minEffectLength = 5.0f;
	static float fractionSpeed = 3.0f;

	camera.offset = { settings::screenWidth / 2.0f, settings::screenHeight / 2.0f };
	Vector2 diff = Vector2Subtract(player->pos(), camera.target);
	float length = Vector2Length(diff);

	if (length > minEffectLength)
	{
		float speed = fmaxf(fractionSpeed * length, minSpeed);
		camera.target = Vector2Add(camera.target, Vector2Scale(diff, speed * dt / length));
	}

	auto levelSize = LevelMgr->currentLdtkLevel->size;
	float minX = 0.0f, minY = 0.0f, maxX = levelSize.x , maxY = levelSize.y ;
	Vector2 max = GetWorldToScreen2D({ maxX, maxY }, camera);
	Vector2 min = GetWorldToScreen2D({ minX, minY }, camera);
	if (max.x < settings::screenWidth)
	{
		camera.offset.x = settings::screenWidth - (max.x - settings::screenWidth / 2);
		player_focused_cam.offset.x = settings::screenWidth - (max.x - settings::screenWidth / 2);
	}
	if (max.y < settings::screenHeight)
	{
		camera.offset.y = settings::screenHeight - (max.y - settings::screenHeight / 2);
		player_focused_cam.offset.y = settings::screenHeight - (max.y - settings::screenHeight / 2);
	}
	if (min.x > 0)
	{
		camera.offset.x = settings::screenWidth / 2 - min.x;
		player_focused_cam.offset.x = settings::screenWidth / 2 - min.x;
	}
	if (min.y > 0)
	{
		camera.offset.y = settings::screenHeight / 2 - min.y;
		player_focused_cam.offset.y = settings::screenHeight / 2 - min.y;
	}


}

Screens GameScreen::Update(float dt)
{
	//full screen at current screensize
	if (IsKeyPressed(KEY_F11) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT))
	{
		ToggleFullscreen();
	}

	if (IsKeyPressed(KEY_ONE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		debug = !debug;
	}

	UpdateCamera(dt);
	player->Update(dt);
	LevelMgr->Update(dt);
	EnitityManager::Update(dt);
	
	return Screens::NONE;
}

void GameScreen::Draw()
{
	BeginMode2D(camera);		// DRAW ORDER:
	LevelMgr->Draw();			// Level layers (Static Background -> Paralax Background -> Solid tiles -> Level Decorations)
	EnitityManager::Draw(0);	// Entities/Objects behind player
	//TODO						// Entities Shaders?
	player->Draw();				// Player		
	EnitityManager::Draw(1);	// Entities/Objects in front of player	
	LevelMgr->DrawSpotLights();	// Darkness and lights
	LevelMgr->DrawForeGround();	// Paralaxed foreground Level layer
	

	//DEBUG:
	if (debug)
	{
		////player states
		//std::string stateStr = "State: " + player->StatesStrMap[player->state];
		//DrawText(stateStr.c_str(), player->center_pos().x, player->center_pos().y -50, 20, BLACK);

		
		DrawText(std::to_string(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X)).c_str(), player->center_pos().x, player->center_pos().y - 50, 20, BLACK);
		////player animations
		//std::string animStr = "Anim: " + player->animations->m_CurrentActiveAnimation + " :: " +
		//	std::to_string(player->animations->GetAnimation(player->animations->m_CurrentActiveAnimation)->GetCurrentFrameNum());
		//DrawText(animStr.c_str(), player->x, player->y-70, 20, BLACK);
		//player contacts
		//DrawText(player->contact_debug_str.c_str(), player->pos().x, player->pos().y - 100, 1, GREEN);



		auto world = LevelMgr->world;
		auto currentBody = world->GetBodyList();
		while (currentBody != nullptr)
		{
			auto pos = currentBody->GetPosition();
			DrawCircle(	pos.x * settings::PPM,
						pos.y * settings::PPM,
						2,
						PURPLE);
			auto fixture = currentBody->GetFixtureList();
			while (fixture != nullptr)
			{
				auto shape = fixture->GetShape();
				// Note, right now supposing all shapes are polygons, use to determine shape->GetType();

				if (shape->GetType() == b2Shape::e_chain)
				{
					auto chainShape = (b2ChainShape*)shape;

					int vertexCount = 4; // since we're assuming they're squares
					for (int j = 0; j < vertexCount; j++)
					{
						b2Vec2 vertexA = chainShape->m_vertices[j];

						int jj = (((j + 1) < vertexCount) ? (j + 1) : 0); // Get next vertex or first to close the shape
						b2Vec2 vertexB = chainShape->m_vertices[jj];

						DrawLineV({ (pos.x + vertexA.x) * settings::PPM, (pos.y + vertexA.y) * settings::PPM },
							{ (pos.x + vertexB.x) * settings::PPM, (pos.y + vertexB.y) * settings::PPM },
							ORANGE); // Draw a line between two vertex positions
					}
				}
				else
				{
					auto polygonShape = (b2PolygonShape*)shape;

					int vertexCount = 4; // since we're assuming they're squares
					for (int j = 0; j < vertexCount; j++)
					{
						b2Vec2 vertexA = polygonShape->m_vertices[j];

						int jj = (((j + 1) < vertexCount) ? (j + 1) : 0); // Get next vertex or first to close the shape
						b2Vec2 vertexB = polygonShape->m_vertices[jj];

						DrawLineV({ (pos.x + vertexA.x) * settings::PPM, (pos.y + vertexA.y) * settings::PPM },
							{ (pos.x + vertexB.x) * settings::PPM, (pos.y + vertexB.y) * settings::PPM },
							GREEN); // Draw a line between two vertex positions
					}
				}

				fixture = fixture->GetNext();
			}

			currentBody = currentBody->GetNext();
		}


	}

	EndMode2D();

	//DEBUG:
	if (debug)
	{
		DrawFPS(5, 5);
		std::string txt = "Entity count: " + std::to_string(EnitityManager::EntityList.size());
		DrawText(txt.c_str(), 5, 20, 20, GREEN);
		if (player != nullptr)
		{
			std::string txt2 = "Player POS: " + VecToString(player->pos());
			DrawText(txt2.c_str(), 5, 40, 20, GREEN);
			std::string txt3 = "Is touching floor: " + std::to_string(player->is_touching_floor);
			DrawText(txt3.c_str(), 5, 60, 20, GREEN);
		}
	}
	
}





