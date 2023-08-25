#include "nlohmann/json.hpp"
#include <fstream>
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
#include "WhiteCoatArm.h"
#include "Acid.h"
#include "Terminal.h"
#include "AxePickup.h"
#include "Gate.h"
#include "BossGlass.h"
#include "Switch.h"
#include "MediPod.h"
#include "SecretFog.h"

b2World* LevelManager::world = nullptr;
b2World* Collidable::world = nullptr;
const ldtk::Level* LevelManager::currentLdtkLevel;
float LevelManager::m_darkness_strength;
std::vector<std::unique_ptr<Entity>> LevelManager::level_entities_safe;
std::vector<Entity*> LevelManager::queue_entities;
std::vector<std::unique_ptr<Collidable>> LevelManager::solid_tiles;
using json = nlohmann::json;

void LevelManager::RemoveEntityFromLevel(Entity& e)
{
	e.m_destroy = true;
}

LevelManager::LevelManager(bool is_new_game, int in_save_file_num)
{
	save_file_num = in_save_file_num;

	ldtk::Project* p = new ldtk::Project();
	p->loadFromFile("res\\level\\GameLevels.ldtk");
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

	if (is_new_game)
	{
		LoadLevel("Level_0");
		new_player_pos = { 250, 170 };
	}
	else
	{
		// Load JSON data from the selected slot
		std::ifstream loadFile("save_slot_" + std::to_string(save_file_num + 1) + ".json");
		json save_data;
		if (loadFile.is_open())
		{
			loadFile >> save_data;
			loadFile.close();
		}

		std::string saved_level = save_data["player"]["currentLevel"];
		LoadLevel(saved_level);

		float posX = save_data["player"]["position"][0];
		float posY = save_data["player"]["position"][1];
		new_player_pos = { posX - 16.0f, posY - 16.0f };
	}

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

	//Level ambient particles
	ParticlesManager::UpdateTextureSize(currentLdtkLevel->size.x, currentLdtkLevel->size.y);
	level_particles_on = currentLdtkLevel->getField<bool>("Particles").value();
	if (level_particles_on)
	{
		level_ambient_particles = new ParticleEmitter(Vector2{ (float)levelSize.x / 2, (float)levelSize.y / 2 });
		ParticlesManager::Add(DefinedEmitter::ambient_particles, level_ambient_particles);
		level_particles_foreground = new ParticleEmitter(Vector2{ (float)levelSize.x / 2, (float)levelSize.y / 2 });
		ParticlesManager::Add(DefinedEmitter::ambient_particles_foreground, level_particles_foreground);
	}
	//Level fog
	level_fog_on = currentLdtkLevel->getField<bool>("Fog").value();  

	GameScreen::lock_camera = currentLdtkLevel->getField<bool>("LockCamera").value();
	if(GameScreen::lock_camera)
	{
		GameScreen::lock_cam_x = currentLdtkLevel->getArrayField<float>("CamTarget")[0].value();
		GameScreen::lock_cam_y = currentLdtkLevel->getArrayField<float>("CamTarget")[1].value();
	}

	  
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

	SaveLevel();

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
	//
	//perlin
	if (level_fog_on)
	{
		int max = std::max(levelSize.x, levelSize.y);
		GameScreen::shaders->RenderPerlinTexture = LoadRenderTexture(max, max);
		GameScreen::shaders->PerlinTexture = GameScreen::shaders->RenderPerlinTexture.texture;
	}


	//Level elements in front of player
	decorationRenderTextureFront = LoadRenderTexture(levelSize.x, levelSize.y);


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
					DrawTextureRec(laboratorySolidsSpriteAtlas, source_rect, target_pos, WHITE);
				}

				if (layer.getName() == "Slopes")
				{
					Rectangle rec = { (float)tile.getPosition().x ,
									 (float)tile.getPosition().y + 8 ,
									 tile_size , tile_size - 8 };
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
		if (layer.getName() == "Platforms")
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
				Rectangle rec = { (float)tile.getPosition().x ,
								 (float)tile.getPosition().y ,
								 tile_size , tile_size };
				Vector2 target_pos = {
								(float)tile.getPosition().x,
								(float)tile.getPosition().y,
				};
				DrawTextureRec(decorationSpriteAtlas, source_rect, target_pos, WHITE);
			}
		}

	}

	EndTextureMode();
	decorationRenderedLevelTexture = decorationRenderTexture.texture;

	//FrontPlayerDecorElements
	Shader darken_shader = LoadShader(0, TextFormat("res/shaders/glsl%i/darken.fs", GLSL_VERSION));
	int darknessFactorLoc = GetShaderLocation(darken_shader, "darknessFactor");
	float darkness_factor = 0.6f;
	SetShaderValue(darken_shader, darknessFactorLoc, &darkness_factor, SHADER_UNIFORM_FLOAT);
	BeginTextureMode(decorationRenderTextureFront);
	BeginShaderMode(darken_shader);
	for (auto&& layer : currentLdtkLevel->allLayers())
	{
		auto& tileset = layer.getTileset();
		if (layer.getName() == "FrontPlayerDecorElements")
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
				Rectangle rec = { (float)tile.getPosition().x ,
								 (float)tile.getPosition().y ,
								 tile_size , tile_size };
				Vector2 target_pos = {
								(float)tile.getPosition().x,
								(float)tile.getPosition().y,
				};
				DrawTextureRec(decorationSpriteAtlas, source_rect, target_pos, WHITE);
			}
		}
	}
	EndShaderMode();
	EndTextureMode();
	decorationRenderedLevelTextureFront = decorationRenderTextureFront.texture;
	UnloadShader(darken_shader);

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

		if (entity.getName() == "SecretFog")
		{
			level_entities_safe.push_back(std::make_unique<SecretFog>(rect));
			level_entities_safe.back().get()->m_ldtkID = entity.iid;
			level_entities_safe.back().get()->m_draw_layers = 0;
		}

		if (entity.getName() == "Door")
		{
			Rectangle r = { (float)entity.getPosition().x + 8.0f,
					(float)entity.getPosition().y -4,
					(float)entity.getSize().x ,
					(float)entity.getSize().y +8};

			level_entities_safe.push_back(std::make_unique<Door>(r, true));

			Door* door = static_cast<Door*>(level_entities_safe.back().get());

			door->locked = entity.getField<bool>("Locked").value();
			door->m_draw_layers = 0;
			door->refs = entity.getArrayField<ldtk::EntityRef>("references");
		}
		if (entity.getName() == "Elevator")
		{
			Rectangle r = { (float)entity.getPosition().x,
					(float)entity.getPosition().y + 4,
					(float)entity.getSize().x ,
					(float)entity.getSize().y };
			level_entities_safe.push_back(std::make_unique<Elevator>(r, entity.getArrayField<int>("Levels"), entity));
			Elevator* e = static_cast<Elevator*>(level_entities_safe.back().get());
			e->m_draw_layers = 1;
			e->m_ldtkID = entity.iid;
			e->powered = entity.getField<bool>("Powered").value();
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
			bool on = entity.getField<bool>("On").value();
			bool flickering = entity.getField<bool>("Flickering").value();
			Color color = { c.r, c.g, c.b, c.a };

			lights->SetupLight(rect.x + rect.width * 0.25f, rect.y + rect.height / 2, in_radius, out_radius, color, is_color, is_dynamic, on, flickering);
			lights->SetupLight(rect.x + rect.width * 0.75f, rect.y + rect.height / 2, in_radius, out_radius, color, is_color, is_dynamic, on, flickering);
		}
		if (entity.getName() == "Light32")
		{
			float in_radius = entity.getField<float>("InRadius").value();
			float out_radius = entity.getField<float>("OutRadius").value();
			bool is_color = entity.getField<bool>("isColor").value();
			bool is_dynamic = entity.getField<bool>("isDynamic").value();
			auto c = entity.getField<ldtk::Color>("Color").value();
			bool on = entity.getField<bool>("On").value();
			bool flickering = entity.getField<bool>("Flickering").value();
			Color color = { c.r, c.g, c.b, c.a };

			lights->SetupLight(rect.x + rect.width / 2, rect.y + rect.height / 2, in_radius, out_radius, color, is_color, is_dynamic, on, flickering);
		}
		if (entity.getName() == "MovingBlock")
		{
			Rectangle r = { (float)entity.getPosition().x,
					(float)entity.getPosition().y + 4,
					(float)entity.getSize().x ,
					(float)entity.getSize().y };
			level_entities_safe.push_back(std::make_unique<MovingBlock>(r, entity.getArrayField<ldtk::IntPoint>("Path")));
		}
		if (entity.getName() == "WoodCrate")
		{
			level_entities_safe.push_back(std::make_unique<WoodCrate>(rect));
		}
		if (entity.getName() == "MediPod")
		{
			level_entities_safe.push_back(std::make_unique<MediPod>(rect));
		}
		if (entity.getName() == "Gate")
		{
			level_entities_safe.push_back(std::make_unique<Gate>(rect));
		}
		if (entity.getName() == "Terminal")
		{
			level_entities_safe.push_back(std::make_unique<Terminal>(rect));
			level_entities_safe.back().get()->m_ldtkID = entity.iid;
		}
		if (entity.getName() == "Switch")
		{
			level_entities_safe.push_back(std::make_unique<Switch>(rect));
			Switch* sw = static_cast<Switch*>(level_entities_safe.back().get());
			sw->m_ldtkID = entity.iid;
			sw->m_elevator_reference = entity.getField<ldtk::EntityRef>("reference").value().entity_iid;
		}
		if (entity.getName() == "AxePickup")
		{
			level_entities_safe.push_back(std::make_unique<AxePickup>(rect));
		}
		if (entity.getName() == "Acid")
		{
			Rectangle r = { (float)entity.getPosition().x,
					(float)entity.getPosition().y + 12,
					(float)entity.getSize().x ,
					(float)entity.getSize().y };
			level_entities_safe.push_back(std::make_unique<Acid>(r));
		}
		if (entity.getName() == "BossGlass")
		{
			level_entities_safe.push_back(std::make_unique<BossGlass>(rect));
			level_entities_safe.back().get()->m_ldtkID = entity.iid;
		}
		if (entity.getName() == "FireAxe")
		{
			level_entities_safe.push_back(std::make_unique<FireAxe>(rect));
		}
		if (entity.getName() == "PlatformBox")
		{
			Rectangle r = { (float)entity.getPosition().x,
					(float)entity.getPosition().y + 4,
					(float)entity.getSize().x ,
					(float)entity.getSize().y - 4 };
			level_entities_safe.push_back(std::make_unique<Platform>(r));
		}
		if (entity.getName() == "InfectedHazmat")
		{
			level_entities_safe.push_back(std::make_unique<InfectedHazmat>(rect, entity.getArrayField<ldtk::IntPoint>("Path")));
			level_entities_safe.back().get()->m_draw_layers = 1;
		}
		if (entity.getName() == "WCArm")
		{
			level_entities_safe.push_back(std::make_unique<WhiteCoatArm>(rect));
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
		if (entity.getName() == "HeadSpit")
		{

			level_entities_safe.push_back(std::make_unique<HeadSpit>(rect));
			HeadSpit* hs = static_cast<HeadSpit*>(level_entities_safe.back().get());
			hs->upsidedown = entity.getField<bool>("upsidedown").value();
			hs->m_draw_layers = 1;
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
			std::string target_lvl = entity.getField<std::string>("ToLevelStr").value();
			ldtk::IID portal_out = entity.getField<ldtk::EntityRef>("LevelPortal_out").value().entity_iid;
			level_entities_safe.push_back(std::make_unique<LevelPortal>(rect, target_lvl, portal_out));
			
		}
		if (entity.getName() == "Footb")
		{
			Rectangle r = { (float)entity.getPosition().x + (float)entity.getSize().x / 2,
					(float)entity.getPosition().y - 4,
					(float)entity.getSize().x ,
					(float)entity.getSize().y };
			level_entities_safe.push_back(std::make_unique<Football>(r));
			level_entities_safe.back().get()->m_draw_layers = 1;
		}
	}
	
	//Light walls
	lights->SetupBoxes();


	LoadSavedLevel();

}

void LevelManager::SaveLevel()
{
	if (GameScreen::player != nullptr)
	{
		std::ifstream loadFile("save_slot_" + std::to_string(save_file_num + 1) + ".json");
		json save_data;
		if (loadFile.is_open())
		{
			loadFile >> save_data;
			loadFile.close();
		}

		// Update the current level and player position
		//save_data["player"]["currentLevel"] = currentLdtkLevel->name;
		//save_data["player"]["position"] = { new_player_pos.x,  new_player_pos.y };

		save_data["level_data"][currentLdtkLevel->name]["visited"] = true;

		//Used Dialogue Lines
		json usedLinesArray = json::array();
		for (int line : DialogueManager::used_lines) {
			usedLinesArray.push_back(line);
		}
		save_data["dialogue_used_lines"] = usedLinesArray;

		// Save the updated JSON data back to the file
		std::ofstream outputFile("save_slot_" + std::to_string(save_file_num + 1) + ".json");
		if (outputFile.is_open()) {
			outputFile << save_data;
			outputFile.close();
			std::cout << "Data saved successfully." << std::endl;
		}
		else {
			std::cerr << "Failed to open file for writing." << std::endl;
		}
	}
}

void LevelManager::LoadSavedLevel()
{
	std::ifstream loadFile("save_slot_" + std::to_string(save_file_num + 1) + ".json");
	json save_data;
	if (loadFile.is_open())
	{
		loadFile >> save_data;
		loadFile.close();
	}

	float posX = save_data["player"]["position"][0];
	float posY = save_data["player"]["position"][1];
	new_player_pos = { posX - 16.0f, posY - 16.0f };


	//Load Dialogues
	json usedLinesArray = save_data["dialogue_used_lines"];
	// Convert the JSON array values to integers and populate used_lines vector
	DialogueManager::used_lines.clear(); // Clear existing data
	for (const auto& value : usedLinesArray) {
		DialogueManager::used_lines.push_back(value);
	}

	//Load Level Objects
	json levelData = save_data["level_data"];
	// Iterate through each level in the level_data object
	for (json::iterator it = levelData.begin(); it != levelData.end(); ++it) {
		std::string levelName = it.key(); // Get the level name

		if (currentLdtkLevel->name == levelName)
		{
			// Access the current level object
			json levelObject = it.value();


			// Check if "terminals" key exists in the current level object
			if (levelObject.find("terminals") != levelObject.end()) {
				// Access the terminals object for the current level
				json terminalsObject = levelObject["terminals"];

				// Iterate through each terminal in the terminals object
				for (json::iterator termIt = terminalsObject.begin(); termIt != terminalsObject.end(); ++termIt) {
					std::string terminalID = termIt.key(); // Get the terminal ID
					bool terminalValue = termIt.value();   // Get the terminal value (true/false)

					// Print the terminal ID and value
					std::cout << "Level: " << levelName << ", Terminal ID: " << terminalID
						<< ", ON: " << (terminalValue ? "true" : "false") << std::endl;

					for (auto& e : level_entities_safe)
					{
						if (e.get()->m_ldtkID.str() == terminalID)
						{
							Terminal* t = static_cast<Terminal*>(e.get());
							t->state = terminalValue ? TerminalState::Pass : TerminalState::Idle;
						}
					}
				}
			}

			// Check if "switches" key exists in the current level object
			if (levelObject.find("switches") != levelObject.end()) {
				// Access the terminals object for the current level
				json switchesObject = levelObject["switches"];

				// Iterate through each terminal in the terminals object
				for (json::iterator switchIt = switchesObject.begin(); switchIt != switchesObject.end(); ++switchIt) {
					std::string swtichID = switchIt.key(); // Get the terminal ID
					bool switchValue = switchIt.value();   // Get the terminal value (true/false)

					// Print the terminal ID and value
					std::cout << "Level: " << levelName << ", Switch ID: " << swtichID
						<< ", ON: " << (switchValue ? "true" : "false") << std::endl;

					for (auto& e : level_entities_safe)
					{
						if (e.get()->m_ldtkID.str() == swtichID)
						{
							Switch* t = static_cast<Switch*>(e.get());
							t->state = switchValue ? SwitchState::On : SwitchState::Off;
						}
					}
				}
			}

			// Check if "secrets" key exists in the current level object
			if (levelObject.find("secrets") != levelObject.end()) {
				// Access the terminals object for the current level
				json secretObject = levelObject["secrets"];

				// Iterate through each terminal in the terminals object
				for (json::iterator secretIt = secretObject.begin(); secretIt != secretObject.end(); ++secretIt) {
					std::string secretID = secretIt.key(); // Get the terminal ID
					bool secretValue = secretIt.value();   // Get the terminal value (true/false)

					// Print the terminal ID and value
					std::cout << "Level: " << levelName << ", Switch ID: " << secretID
						<< ", ON: " << (secretValue ? "true" : "false") << std::endl;

					for (auto& e : level_entities_safe)
					{
						if (e.get()->m_ldtkID.str() == secretID)
						{
							SecretFog* s = static_cast<SecretFog*>(e.get());
							s->revealed = secretValue ? true : false;
							s->sound_played = s->revealed ? true : false;
						}
					}
				}
			}
			// Check if "boss" key exists in the current level object
			if (levelObject.find("boss") != levelObject.end()) {
				// Access the terminals object for the current level
				json bossObject = levelObject["boss"];

				// Iterate through each terminal in the terminals object
				for (json::iterator bossIt = bossObject.begin(); bossIt != bossObject.end(); ++bossIt) {
					std::string bossID = bossIt.key(); // Get the boss ID
					bool bossValue = bossIt.value();   // Get the boss value (true/false)

					// Print the terminal ID and value
					std::cout << "Level: " << levelName << ", Boss ID: " << bossID
						<< ", ON: " << (bossValue ? "true" : "false") << std::endl;

					for (auto& e : level_entities_safe)
					{
						if (e.get()->m_ldtkID.str() == bossID)
						{
							BossGlass* bs = static_cast<BossGlass*>(e.get());
							bs->state = bossValue ? BossGlassState::Shattered : BossGlassState::Idle;
							bs->level_unlocked = bossValue ? true : false;
							GameScreen::lock_camera = bossValue ? false : true;
						}
					}
				}
			}
		}

		
	}

}

void LevelManager::UnloadLevel()
{
	GameScreen::Particles->UnloadEmitters();
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
	if (level_particles_on)
	{
		level_ambient_particles->set_forever(false);
		level_ambient_particles = nullptr;
		level_particles_foreground->set_forever(false);
		level_particles_foreground = nullptr;
	}


	UnloadTexture(laboratorySolidsRenderedLevelTexture);
	UnloadTexture(paralaxedBackgroundRenderedLevelTexture);
	UnloadTexture(paralaxedForegroundRenderedLevelTexture);
	UnloadTexture(baseBackgroundSpriteAtlas);
	UnloadTexture(decorationRenderedLevelTexture);
	UnloadTexture(decorationSpriteAtlas);
	UnloadTexture(paralaxedBackgroundSpriteAtlas);
	UnloadTexture(paralaxedForegroundSpriteAtlas);
	UnloadTexture(decorationRenderedLevelTextureFront);
	UnloadTexture(GameScreen::shaders->PerlinTexture);
	UnloadRenderTexture(GameScreen::shaders->RenderPerlinTexture);
	UnloadRenderTexture(decorationRenderTextureFront);
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

	StopSound(SoundManager::sounds["light_ambient"]);
	
}

void LevelManager::ResetLevel()
{
	next_level = currentLdtkLevel->name;
}

void LevelManager::Update(float dt)
{

	const float timeStep = dt;
	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;
	world->Step(timeStep, velocityIterations, positionIterations);

	lights->UpdateLights(dt);
	if(level_particles_on) level_ambient_particles->position(GameScreen::player->center_pos());

	for (auto& e : level_entities_safe)
	{
		if(e->queue_entity_add)
		EnitityManager::Add(e.get());
		e->queue_entity_add = false;
	}
	if (next_level != "")
	{
		LoadLevel(next_level);
		next_level = "";
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
			int tid = layer.getTile(x,y).tileId;	
			bool tid_floor = ((tid < 8 && tid > -1) || tid == 15 || tid == 16 || tid == 38 || tid == 39);
			
			if (!tid_floor && tid != -1)
			{
				int x_counter = 0;
				bool next = true;
				while (next)
				{
					int next_tid = layer.getTile(x + x_counter, y).tileId;
					bool next_tid_floor = ((next_tid < 8 && next_tid > -1) || next_tid == 15 || next_tid == 16 || next_tid == 38 || next_tid == 39);
					if (next_tid == -1 || x + x_counter >= grid_x || next_tid_floor)
					{
						next = false;
						x_line.push_back({ (float)x * settings::tileSize,
											(float)y * settings::tileSize,
											float(x_counter * settings::tileSize),
											settings::tileSize });
						x += x_counter-1;
					}
					else
					{
						x_counter++;
					}
				}
			}
			else if (tid_floor)
			{
				int x_fcounter = 0;
				bool next = true;
				while (next)
				{
					int next_tid = layer.getTile(x + x_fcounter, y).tileId;
					bool next_tid_floor = ((next_tid < 8 && next_tid > -1) || next_tid == 15 || next_tid == 16 || next_tid == 38 || next_tid == 39);
					if (next_tid == -1 || x + x_fcounter >= grid_x || !next_tid_floor)
					{
						next = false;
						x_line.push_back({	(float)x * settings::tileSize,
											(float)y * settings::tileSize + 4,
											float(x_fcounter * settings::tileSize),
											settings::tileSize -4 });
						x += x_fcounter-1;
					}
					else
					{
						x_fcounter++;
					}
				}
			} 
			
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
		DrawRectangleLinesEx(r, 1, BLUE);
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

void LevelManager::DrawInFrontOfPlayer()
{
	DrawTexturePro(decorationRenderedLevelTextureFront,
		{ 0, 0, (float)decorationRenderedLevelTextureFront.width, (float)-decorationRenderedLevelTextureFront.height },
		{ 0, 0, (float)levelSize.x ,(float)levelSize.y },
		{ 0,0 }, 0, WHITE);
}

void LevelManager::Draw()
{

	Player& p = *GameScreen::player;
	Camera2D c = GameScreen::camera;


	Vector2 c_position = { (c.offset.x / c.zoom - c.target.x) , (c.offset.y / c.zoom - c.target.y) };

	Vector2 parallaxed = Vector2Multiply(c_position, { 0.05f,0.05f });
	Vector2 parallaxed_far = Vector2Multiply(c_position, { 0.02f,0.02f });


	////Draw static background
	if (currentLdtkLevel->hasBgImage())
	{
		DrawTexturePro(baseBackgroundRenderedLevelTexture,
			{ 0, 0, (float)baseBackgroundRenderedLevelTexture.width, (float)-baseBackgroundRenderedLevelTexture.height },
			{ 0, 0, (float)levelSize.x ,(float)levelSize.y  },
			{ 0,0 }, 0, WHITE);
	}

	//Draw paralaxed background elements
	DrawTexturePro(paralaxedBackgroundRenderedLevelTexture,
		{ 0, 0, (float)paralaxedBackgroundRenderedLevelTexture.width, (float)-paralaxedBackgroundRenderedLevelTexture.height },//source
		{ parallaxed_far.x ,parallaxed_far.y, (float)levelSize.x ,(float)levelSize.y  }, //dest
		{ 0,0 }, 0, WHITE);

	//Draw level solids
	DrawTexturePro(laboratorySolidsRenderedLevelTexture,
		{ 0, 0, (float)laboratorySolidsRenderedLevelTexture.width, (float)-laboratorySolidsRenderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x ,(float)levelSize.y  }, 
		{ 0,0 }, 0, WHITE);


	//Draw decorations
	DrawTexturePro(decorationRenderedLevelTexture,
		{ 0, 0, (float)laboratorySolidsRenderedLevelTexture.width, (float)-laboratorySolidsRenderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x ,(float)levelSize.y  },
		{ 0,0 }, 0, WHITE);



}


