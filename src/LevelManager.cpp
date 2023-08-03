#include "LevelManager.h"
#include "Collidable.h"
#include "Door.h"
#include <raymath.h>
#include "SolidTile.h"
#include "Settings.h"
#include "GameScreen.h"
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/World.hpp>
#include <LDtkLoader/Level.hpp>
#include <iostream>
#include <string>
#include "AnimatedDecor.h"
#include "Elevator.h"
#include "ElevatorCallSwitch.h"
#include "MovingBlock.h"
#include "WoodCrate.h"
#include "Platform.h"
#include "InfectedHazmat.h"
#include "FireAxe.h"
#include "ContactListener.h"
#include "FlyingInfected.h"
#include "LightManager.h"
#include "Ribbs.h"
#include "Leggy.h"
#include "Football.h"
#include "HeadSpit.h"
#include "NPCSecurityGuy.h"
#include "NPCSecurityGuy2.h"


b2World* LevelManager::world = nullptr;
b2World* Collidable::world = nullptr;
const ldtk::Level* LevelManager::currentLdtkLevel;
float LevelManager::m_darkness_strength;
std::vector<std::unique_ptr<Entity>> LevelManager::level_entities_safe;
std::vector<Entity*> LevelManager::queue_entities;
std::vector<std::unique_ptr<Collidable>> LevelManager::solid_tiles;


void LevelManager::RemoveEntityFromLevel(Entity& e)
{
	e.m_destroy = true;
}

LevelManager::LevelManager()
{
	
	ldtk::Project* p = new ldtk::Project();
	p->loadFromFile("res\\level\\TestLevel.ldtk");
	ldtkProject = p;

	laboratorySolidsSpriteAtlas = LoadTexture("res\\level\\lab4.png");
	ldtkWorld = &ldtkProject->getWorld();
	contact_filter = new ContactFilter();
	contacts = new ContactListener();
	destruction_listener = new DestructionListener();
	world = new b2World(gravity);
	world->SetContactListener(contacts);
	world->SetDestructionListener(destruction_listener);
	world->SetContactFilter(contact_filter);
	Collidable::world = world;
	LoadLevel("Level_0");

}

LevelManager::~LevelManager()
{
}

void LevelManager::LoadLevel(std::string level_name)

{
	if (currentLdtkLevel != nullptr)
	{
		UnloadLevel();
	}

	//Set to new current level
	currentLdtkLevel = &ldtkWorld->getLevel(level_name);
	levelSize = currentLdtkLevel->size;
	lights = new LightManager();
	m_darkness_strength = currentLdtkLevel->getField<float>("DarknessLevel").value();

	if (world == nullptr)
	{
		contacts = new ContactListener();
		destruction_listener = new DestructionListener();
		contact_filter = new ContactFilter();
		world = new b2World(gravity);
		world->SetContactListener(contacts);
		world->SetDestructionListener(destruction_listener);
		world->SetContactFilter(contact_filter);
	}
	Collidable::world = world;

	if (GameScreen::player != nullptr)
	{
		GameScreen::player->RecreateBody();
		GameScreen::player->NewBody();

		GameScreen::camera.target = new_player_pos;
		GameScreen::player->m_body->SetTransform({ new_player_pos.x / settings::PPM, new_player_pos.y / settings::PPM }, 0);

	}


	lights->m_lights.emplace_back();

	lights->SetupBoxes();

	//Textures setup:
	// 
	//Background Texture
	std::string path = currentLdtkLevel->getBgImage().path.c_str();
	std::string prefix = "res/level/";
	std::string final = prefix + path;
	baseBackgroundSpriteAtlas = LoadTexture(final.c_str());
	baseBackgroundRenderTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	//
	//Tiles Texture
	laboratorySolidsRenderTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	//
	//Paralax Textures
	paralaxBackgroundRenderTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	paralaxedBackgroundSpriteAtlas = LoadTexture("res//level//bg.png");

	paralaxedForegroundRenderTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	paralaxedForegroundSpriteAtlas = LoadTexture("res//level//mothman.png");
	//
	//Decoration Texture
	decorationSpriteAtlas = LoadTexture("res//level//mothman.png");
	decorationRenderTexture = LoadRenderTexture(levelSize.x, levelSize.y);

	
	//Draw render texture logic and objects setup
	
	//Static background draw
	BeginTextureMode(baseBackgroundRenderTexture);
	if (currentLdtkLevel->hasBgImage())
	{
		// tile background texture to cover the whole frame buffer
		for (int i = 0; i <= (levelSize.x / baseBackgroundSpriteAtlas.width); i++)
		{
			for (int j = 0; j <= (levelSize.y / baseBackgroundSpriteAtlas.height); j++)
			{
				DrawTextureV(baseBackgroundSpriteAtlas, { float(i * baseBackgroundSpriteAtlas.width), float(j * baseBackgroundSpriteAtlas.height) }, WHITE);
			}
		}
	}
	EndTextureMode();
	baseBackgroundRenderedLevelTexture = baseBackgroundRenderTexture.texture;

	//Tiles Draw
	BeginTextureMode(laboratorySolidsRenderTexture);
	for (auto&& layer : currentLdtkLevel->allLayers())
	{
		if (layer.hasTileset())
		{
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
					(float)tile.getPosition().y
				};

				//collision rectangles
				if (layer.getName() == "LabSolids")
				{
					Rectangle rec = { tile.getPosition().x, tile.getPosition().y, tile_size, tile_size };
					//solid_tiles.push_back(std::make_unique<SolidTile>(rec));
					DrawTextureRec(laboratorySolidsSpriteAtlas, source_rect, target_pos, WHITE);
				}
				if (layer.getName() == "Platforms")
				{
					Rectangle rec = {(float)tile.getPosition().x ,
									 (float)tile.getPosition().y ,
									 tile_size , tile_size };
					//solid_tiles.push_back(std::make_unique<SolidTile>(rec));
					DrawTextureRec(decorationSpriteAtlas, source_rect, target_pos, WHITE);
				}
				if (layer.getName() == "Slopes")
				{
					Rectangle rec = { (float)tile.getPosition().x ,
									 (float)tile.getPosition().y ,
									 tile_size , tile_size };
					if (tile.tileId == 881)
					{
						b2Vec2 verices[3] = { {-1.0f,-1.0f},{1.0f,1.0f},{-1.0f, 1.0f}};
						solid_tiles.push_back(std::make_unique<SolidTile>(verices, 3, rec));
						
					}
					if (tile.tileId == 884)
					{
						b2Vec2 verices[3] = {{-1.0f, 1.0f},{1.0f,-1.0f}, {1.0f,1.0f} };
						solid_tiles.push_back(std::make_unique<SolidTile>(verices, 3, rec));

					}
					DrawTextureRec(decorationSpriteAtlas, source_rect, target_pos, WHITE);
				}
			}
		}
	}

	EndTextureMode();
	laboratorySolidsRenderedLevelTexture = laboratorySolidsRenderTexture.texture;

	SolidTilesToBigBoxes();

	//Decoration Draw
	BeginTextureMode(decorationRenderTexture);
	for (auto&& layer : currentLdtkLevel->allLayers())
	{
		auto& tileset = layer.getTileset();
		if (layer.hasTileset() && layer.getName() == "Decoration")
		{
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

				if (tileset.getTileCustomData(tile.tileId) == "Animated")
				{
					Rectangle r = {
						target_pos.x,
						target_pos.y,
						tile_size ,
						tile_size 
					};
					level_entities_safe.push_back(std::make_unique<AnimatedDecor>(r,tile.tileId));
				}
				else
				{
					DrawTextureRec(decorationSpriteAtlas, source_rect, target_pos, WHITE);
				}
				

			}
		}
	}

	EndTextureMode();
	decorationRenderedLevelTexture = decorationRenderTexture.texture;


	//Paralax Layers Draw
	BeginTextureMode(paralaxedForegroundRenderTexture);

	for (auto&& layer : currentLdtkLevel->allLayers())
	{
		if (layer.getName() == "FGTiles")
		{
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

				Vector2 target_paralax_pos = {
					(float)tile.getPosition().x ,
					(float)tile.getPosition().y
				};
				DrawTextureRec(paralaxedForegroundSpriteAtlas, source_rect, target_paralax_pos, WHITE);

			}
		}
	}
	EndTextureMode();
	paralaxedForegroundRenderedLevelTexture = paralaxedForegroundRenderTexture.texture;

	//Paralax Layers Draw
	BeginTextureMode(paralaxBackgroundRenderTexture);

	for (auto&& layer : currentLdtkLevel->allLayers())
	{
		if (layer.getName() == "BGTiles")
		{
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

				Vector2 target_paralax_pos = {
					(float)tile.getPosition().x ,
					(float)tile.getPosition().y
				};
				DrawTextureRec(paralaxedBackgroundSpriteAtlas, source_rect, target_paralax_pos, WHITE);

			}
		}
	}
	EndTextureMode();
	paralaxedBackgroundRenderedLevelTexture = paralaxBackgroundRenderTexture.texture;

	
	//Load entities
	for (auto&& entity : currentLdtkLevel->getLayer("Entities").allEntities())
	{
		Rectangle rect = {	(float)entity.getPosition().x,
							(float)entity.getPosition().y,
							(float)entity.getSize().x ,
							(float)entity.getSize().y  };

		if (entity.getName() == "Door")
		{
			bool is_right = entity.getField<bool>("Right").value();
			level_entities_safe.push_back(std::make_unique<Door>(rect, is_right));
			level_entities_safe.back().get()->m_draw_layers = 1;
		}
		if (entity.getName() == "Elevator")
		{
			level_entities_safe.push_back(std::make_unique<Elevator>(rect, entity.getArrayField<int>("Levels"), entity));
			level_entities_safe.back().get()->m_draw_layers = 1;
			level_entities_safe.back().get()->m_ldtkID = entity.iid;;
		}
		if (entity.getName() == "ElevatorCallSwitch")
		{
			ldtk::IID elev_reference = entity.getField<ldtk::EntityRef>("Entity_ref").value().entity_iid;
			level_entities_safe.push_back(std::make_unique<ElevatorCallSwitch>(rect, elev_reference));
		}
		if (entity.getName() == "Light64")
		{
			float in_radius = entity.getField<float>("InRadius").value();
			float out_radius = entity.getField<float>("OutRadius").value();
			bool is_color = entity.getField<bool>("isColor").value();
			bool is_dynamic = entity.getField<bool>("isDynamic").value();
			auto c = entity.getField<ldtk::Color>("Color").value();
			Color color = { c.r, c.g, c.b, c.a };

			lights->SetupLight(rect.x + rect.width * 0.25f, rect.y + rect.height / 2, in_radius, out_radius, color, is_color, is_dynamic);
			lights->SetupLight(rect.x + rect.width * 0.75f, rect.y + rect.height / 2, in_radius, out_radius, color, is_color, is_dynamic);
		}
		if (entity.getName() == "Light32")
		{
			float in_radius = entity.getField<float>("InRadius").value();
			float out_radius = entity.getField<float>("OutRadius").value();
			bool is_color = entity.getField<bool>("isColor").value();
			bool is_dynamic = entity.getField<bool>("isDynamic").value();
			auto c = entity.getField<ldtk::Color>("Color").value();
			Color color = { c.r, c.g, c.b, c.a };

			lights->SetupLight(rect.x + rect.width / 2, rect.y + rect.height / 2, in_radius, out_radius, color, is_color, is_dynamic);
		}
		if (entity.getName() == "MovingBlock")
		{
			level_entities_safe.push_back(std::make_unique<MovingBlock>(rect, entity.getArrayField<ldtk::IntPoint>("Path")));
		}
		if (entity.getName() == "WoodCrate")
		{
			level_entities_safe.push_back(std::make_unique<WoodCrate>(rect));
		}
		if (entity.getName() == "FireAxe")
		{
			level_entities_safe.push_back(std::make_unique<FireAxe>(rect));
		}
		if (entity.getName() == "PlatformBox")
		{
			level_entities_safe.push_back(std::make_unique<Platform>(rect));
		}
		if (entity.getName() == "InfectedHazmat")
		{
			level_entities_safe.push_back(std::make_unique<InfectedHazmat>(rect));
			level_entities_safe.back().get()->m_draw_layers = 1;
		}
		if (entity.getName() == "Ribbs")
		{
			level_entities_safe.push_back(std::make_unique<Ribbs>(rect));
			level_entities_safe.back().get()->m_draw_layers = 1;
		}
		if (entity.getName() == "Leggy")
		{
			level_entities_safe.push_back(std::make_unique<Leggy>(rect));
			level_entities_safe.back().get()->m_draw_layers = 1;
		}
		if (entity.getName() == "Footb")
		{
			level_entities_safe.push_back(std::make_unique<Football>(rect));
			level_entities_safe.back().get()->m_draw_layers = 1;
		}
		if (entity.getName() == "HeadSpit")
		{
			level_entities_safe.push_back(std::make_unique<HeadSpit>(rect));
			level_entities_safe.back().get()->m_draw_layers = 1;
		}
		if (entity.getName() == "FlyingInfected")
		{
			level_entities_safe.push_back(std::make_unique<FlyingInfected>(rect));
			level_entities_safe.back().get()->m_draw_layers = 1;
		}
		if (entity.getName() == "NPCSec1")
		{
			level_entities_safe.push_back(std::make_unique<NPCSecurityGuy>(rect));
		}
		if (entity.getName() == "NPCSec2")
		{
			level_entities_safe.push_back(std::make_unique<NPCSecurityGuy2>(rect));
		}
		if (entity.getName() == "LevelPortal_in")
		{
			rect = {		(float)entity.getPosition().x - rect.width / 2,
							(float)entity.getPosition().y - rect.height / 2,
							(float)entity.getSize().x ,
							(float)entity.getSize().y };
			std::string target_lvl = entity.getField<std::string>("ToLevelStr").value();
			ldtk::IID portal_out = entity.getField<ldtk::EntityRef>("LevelPortal_out").value().entity_iid;
			level_entities_safe.push_back(std::make_unique<LevelPortal>(rect, target_lvl, portal_out));
			
		}
	}
	
	//Light walls
	lights->SetupBoxes();

}

void LevelManager::UnloadLevel()
{

	if (world != nullptr)
	{
		// if we had an old world then delete it and recreate
		// a new one for the new level
		delete contacts;
		contacts = nullptr;
		delete contact_filter;
		contact_filter = nullptr;
		delete destruction_listener;
		destruction_listener = nullptr;
		delete world;
		world = nullptr;
	}


	UnloadTexture(laboratorySolidsRenderedLevelTexture);
	UnloadTexture(paralaxedBackgroundRenderedLevelTexture);
	UnloadTexture(paralaxedForegroundRenderedLevelTexture);
	UnloadTexture(baseBackgroundSpriteAtlas);
	UnloadTexture(decorationRenderedLevelTexture);
	UnloadTexture(decorationSpriteAtlas);
	UnloadTexture(paralaxedBackgroundSpriteAtlas);
	UnloadTexture(paralaxedForegroundSpriteAtlas);
	UnloadRenderTexture(paralaxBackgroundRenderTexture);
	UnloadRenderTexture(paralaxedForegroundRenderTexture);
	UnloadRenderTexture(laboratorySolidsRenderTexture);
	UnloadRenderTexture(decorationRenderTexture);
	UnloadRenderTexture(baseBackgroundRenderTexture);

	UnloadRenderTexture(lights->LightMask);
	for (int i = 0; i < lights->m_lights.size(); i++)
	{
		UnloadRenderTexture(lights->m_lights[i].GlowTexture);
		UnloadRenderTexture(lights->m_lights[i].ShadowMask);
	}
	lights->m_light_walls.clear();
	lights->m_lights.clear();

	level_entities_safe.clear();
	solid_tiles.clear();
}

void LevelManager::Update(float dt)
{

	const float timeStep = dt;
	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;
	world->Step(timeStep, velocityIterations, positionIterations);


	if (next_level != "")
	{
		LoadLevel(next_level);
		next_level = "";
	}
	
	lights->UpdateLights();


	for (auto& e : level_entities_safe)
	{
		if(e->queue_entity_add)
		EnitityManager::Add(e.get());
		e->queue_entity_add = false;
	}
	
	
}


void LevelManager::SolidTilesToBigBoxes()
{
	int tiles = 0;
	std::vector<Rectangle> x_line;
	auto& layer = currentLdtkLevel->getLayer("LabSolids");
	int grid_x = layer.getGridSize().x;
	int grid_y = layer.getGridSize().y;

	for (int y = 0; y < grid_y; y++)
	{
		for (int x = 0; x < grid_x; x++)
		{
			int x_counter = 0;
			if (layer.getIntGridVal(x, y).value != -1)
			{
				bool next = true;
				while (next)
				{
					if (layer.getIntGridVal(x + x_counter, y).value == -1 || x + x_counter >= grid_x)
					{
						next = false;
						x_line.push_back({	(float)x * settings::tileSize,
											(float)y * settings::tileSize,
											float(x_counter * settings::tileSize),
											settings::tileSize });
					}
					else
					{
						x_counter++;
					}
				}
			}
			x += x_counter;
		}
	}

	std::vector<Rectangle> b_boxes;

	for (int i = 0; i < x_line.size(); i++)
	{
		auto& r1 = x_line[i];
		for (auto& r2 : x_line)
		{
			if (r1.x == r2.x && r1.width == r2.width && //if horizontaly alligned
				r1.y + r1.height == r2.y)
			{
				r1.height += 32;
				r2 = { 0,0,0,0 };
			}
		}
		b_boxes.push_back(r1);
	}

	for (auto r : b_boxes)
	{
		solid_tiles.push_back(std::make_unique<SolidTile>(r));
		//DrawRectangleLinesEx(r, 1, BLUE);
	}

}

void LevelManager::DrawForeGround()
{
	Camera2D c = GameScreen::camera;
	Vector2 c_position = { (c.offset.x / c.zoom - c.target.x) , (c.offset.y / c.zoom - c.target.y) };
	Vector2 parallaxed = Vector2Multiply(c_position, { 0.05f,0.05f });
	//Draw paralaxed foreground elements
	DrawTexturePro(paralaxedForegroundRenderedLevelTexture,
		{ 0, 0, (float)paralaxedForegroundRenderedLevelTexture.width, (float)-paralaxedForegroundRenderedLevelTexture.height },//source
		{ parallaxed.x ,parallaxed.y, (float)levelSize.x ,(float)levelSize.y }, //dest
		{ 0,0 }, 0, WHITE);
}

void LevelManager::Draw()
{

	Player& p = *GameScreen::player;
	Camera2D c = GameScreen::camera;


	Vector2 c_position = { (c.offset.x / c.zoom - c.target.x) , (c.offset.y / c.zoom - c.target.y) };

	Vector2 parallaxed = Vector2Multiply(c_position, { 0.05f,0.05f });
	Vector2 parallaxed_far = Vector2Multiply(c_position, { 0.02f,0.02f });

	ClearBackground(BLACK);
	////Draw static background
	if (currentLdtkLevel->hasBgImage())
	{
		DrawTexturePro(baseBackgroundRenderedLevelTexture,
			{ 0, 0, (float)baseBackgroundRenderedLevelTexture.width, (float)-baseBackgroundRenderedLevelTexture.height },
			{ 0, 0, (float)levelSize.x ,(float)levelSize.y  },
			{ 0,0 }, 0, WHITE);
	}
	ClearBackground(BLACK);
	//Draw paralaxed background elements
	DrawTexturePro(paralaxedBackgroundRenderedLevelTexture,
		{ 0, 0, (float)paralaxedBackgroundRenderedLevelTexture.width, (float)-paralaxedBackgroundRenderedLevelTexture.height },//source
		{ parallaxed_far.x ,parallaxed_far.y, (float)levelSize.x ,(float)levelSize.y  }, //dest
		{ 0,0 }, 0, WHITE);
	ClearBackground(BLACK);
	//Draw level solids
	DrawTexturePro(laboratorySolidsRenderedLevelTexture,
		{ 0, 0, (float)laboratorySolidsRenderedLevelTexture.width, (float)-laboratorySolidsRenderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x ,(float)levelSize.y  }, 
		{ 0,0 }, 0, WHITE);
	ClearBackground(BLACK);
	//Draw decorations
	DrawTexturePro(decorationRenderedLevelTexture,
		{ 0, 0, (float)laboratorySolidsRenderedLevelTexture.width, (float)-laboratorySolidsRenderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x ,(float)levelSize.y  },
		{ 0,0 }, 0, WHITE);
	ClearBackground(BLACK);
	

}


