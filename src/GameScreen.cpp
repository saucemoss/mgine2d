#include <raylib.h>
#include "Util.h"
#include "GameScreen.h"
#include "Player.h"
#include "ZSpawner.h"
#include "BigZombie.h"
#include "EntityManager.h"
#include "Ground.h"
#include "Settings.h"
#include "LDtkLoader/Project.hpp"


Player* player = nullptr;
ZSpawner* spawner = nullptr;
BigZombie* bz = nullptr;
b2World* GameScreen::b2world = nullptr;
ldtk::Project ldtk_project;
Camera2D camera;

GameScreen::GameScreen()

{

	// Box2d setup
	if (b2world != nullptr)
	{
		// if we had an old world then delete it and recreate
		// a new one for the new level
		delete b2world;
		b2world = nullptr;
	}
	b2Vec2 gravity(0.0f, 60.0f);
	b2world = new b2World(gravity);


	ldtkProject = new ldtk::Project();

	ldtkProject->loadFromFile("res\\level\\TestLevel.ldtk");
	currentTilesetTexture = LoadTexture("res\\Treasure Hunters\\Palm Tree Island\\Sprites\\Terrain\\Terrain (32x32).png");
	ldtkWorld = &ldtkProject->getWorld();
	currentLdtkLevel = &ldtkWorld->getLevel("Level_0");



	auto testTileLayerTileset = currentLdtkLevel->getLayer("IntGrid").getTileset();
	auto levelSize = currentLdtkLevel->size;
	auto renderTexture = LoadRenderTexture(levelSize.x, levelSize.y);

	BeginTextureMode(renderTexture);

	if (currentLdtkLevel->hasBgImage())
	{
		auto backgroundTexture = LoadTexture("res\\Treasure Hunters\\Palm Tree Island\\Sprites\\Background\\BG Image.png");

		DrawTextureEx(backgroundTexture, { 0, 0 },0,4, WHITE);

	}

	// draw all tileset layers
	for (auto&& layer : currentLdtkLevel->allLayers())
	{
		if (layer.hasTileset())
		{
			int i = 0;
			Ground* ground = new Ground[layer.allTiles().size()];
	
			for (auto&& tile : layer.allTiles())
			{
				auto source_pos = tile.getTextureRect();
				auto tile_size = float(layer.getTileset().tile_size);

				Rectangle source_rect = {
					float(source_pos.x),
					float(source_pos.y),
					(tile.flipX ? -tile_size : tile_size),
					(tile.flipY ? -tile_size : tile_size)
				};

				Vector2 target_pos = {
					(float)tile.getPosition().x,
					(float)tile.getPosition().y,
				};
				auto half_tile = tile_size / 2;
				ground[i] = *(new Ground(b2world));
				ground[i].body->SetTransform({((float)tile.getPosition().x + half_tile) / settings::PhysicsWorldScale * settings::ScreenScale,
											  ((float)tile.getPosition().y + half_tile) / settings::PhysicsWorldScale * settings::ScreenScale }, 0);
				i++;
				DrawTextureRec(currentTilesetTexture, source_rect, target_pos, WHITE);
			}
		}
	}

	EndTextureMode();
	renderedLevelTexture = renderTexture.texture;


	// Test entities

	player = new Player(b2world);



	//Camera init
	camera = { 0 };
	camera.target = player->GetPos();
	camera.offset = { settings::screenWidth / 2.0f, settings::screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = settings::zoom;

}

GameScreen::~GameScreen()
{

}

Screens GameScreen::Update(float dt)
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
	//full screen at current screensize
	if (IsKeyPressed(KEY_F11))
	{
		ToggleFullscreen();
	}

	camera.target = player->GetPos();
	camera.offset = { settings::screenWidth / 2.0f, settings::screenHeight / 2.0f };

	auto levelSize = currentLdtkLevel->size;
	float minX = 0, minY = 0, maxX = levelSize.x * settings::ScreenScale, maxY = levelSize.y * settings::ScreenScale;
	Vector2 max = GetWorldToScreen2D({ maxX, maxY }, camera);
	Vector2 min = GetWorldToScreen2D({ minX, minY }, camera);
	if (max.x < settings::screenWidth) camera.offset.x = settings::screenWidth - (max.x - settings::screenWidth / 2);
	if (max.y < settings::screenHeight) camera.offset.y = settings::screenHeight - (max.y - settings::screenHeight / 2);
	if (min.x > 0) camera.offset.x = settings::screenWidth / 2 - min.x;
	if (min.y > 0) camera.offset.y = settings::screenHeight / 2 - min.y;


	// Box2d 
	const float timeStep = 1.0f / 60.0f;
	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;
	b2world->Step(timeStep, velocityIterations, positionIterations);
	EnitityManager::Update(dt);

	return Screens::NONE;
}

void GameScreen::Draw()
{
	auto levelSize = currentLdtkLevel->size;
	BeginMode2D(camera);
	DrawTexturePro(renderedLevelTexture,
		{ 0, 0, (float)renderedLevelTexture.width, (float)-renderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale }, { 0,0 }, 0, WHITE);
	EnitityManager::Draw();
	EndMode2D();


	//auto levelSize = currentLdtkLevel->size;
	//BeginMode2D(camera);
	//DrawTexturePro(renderedLevelTexture,
	//	{ 0, 0, (float)renderedLevelTexture.width, (float)-renderedLevelTexture.height },
	//	{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale }, { 0,0 }, 0, WHITE);
	//EnitityManager::Draw();
	//EndMode2D();


	DrawFPS(5, 5);
	std::string txt = "Entity count: " + std::to_string(EnitityManager::EntityList.size());
	DrawText(txt.c_str(), 5, 20, 20, BLACK);
	if (player != nullptr)
	{
		std::string txt2 = "Player POS: " + VecToString(player->GetPos());
		DrawText(txt2.c_str(), 5, 40, 20, BLACK);
		std::string txt3 = "Is touching floor: " + std::to_string(player->is_touching_floor);
		DrawText(txt3.c_str(), 5, 60, 20, BLACK);
	}
	
}




