#include "LevelManager.h"
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


LevelManager::LevelManager()
{

	ldtk::Project* p = new ldtk::Project();
	p->loadFromFile("res\\level\\TestLevel.ldtk");
	ldtkProject = p;

	terrainTilesetTexture = LoadTexture("res\\level\\Terrain (32x32).png");
	laboratoryTilesetTexture = LoadTexture("res\\level\\lab2.png");
	bgTilesetTexture = LoadTexture("res\\level\\Big Clouds.png");

	ldtkWorld = &ldtkProject->getWorld();

	currentLdtkLevel = &ldtkWorld->getLevel("Level_0");
	levelSize = currentLdtkLevel->size;
	renderBGTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	

	//level portals 
	
	for (const ldtk::Level& l : ldtkWorld->allLevels())
	{
		for (auto&& e : l.getLayer("Entities").getEntitiesByName("LevelPortal_in"))
		{
			auto& entity = e.get();
			Rectangle rect = { ((float)entity.getPosition().x - (float)entity.getSize().x / 2) * settings::ScreenScale,
							   ((float)entity.getPosition().y - (float)entity.getSize().y / 2 ) * settings::ScreenScale,
							   (float)entity.getSize().x * settings::ScreenScale,
							   (float)entity.getSize().y * settings::ScreenScale };

			std::string target_lvl = entity.getField<std::string>("ToLevelStr").value();
			ldtk::IID portal_out = entity.getField<ldtk::EntityRef>("LevelPortal_out").value().entity_iid;
			
			
			level_portals.emplace_back(new LevelPortal(rect, entity, l.name, target_lvl, portal_out));
		}
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

	currentLdtkLevel = &ldtkWorld->getLevel(level_name);
	levelSize = currentLdtkLevel->size;
	renderTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	renderBGTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	staticBGRenderTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	bgTilesetTexture = LoadTexture("res//level//Big Clouds.png");
	//Decoration 
	DecorTex = LoadTexture("res//level//mothman.png");
	staticDecorRenderTexture = LoadRenderTexture(levelSize.x, levelSize.y);

	std::string path = currentLdtkLevel->getBgImage().path.c_str();
	std::string prefix = "res/level/";
	std::string final = prefix + path;
	backgroundTexture = LoadTexture(final.c_str());

	//Static background draw
	BeginTextureMode(staticBGRenderTexture);
	if (currentLdtkLevel->hasBgImage())
	{
		// tile background texture to cover the whole frame buffer
		for (int i = 0; i <= (levelSize.x / backgroundTexture.width); i++)
		{
			for (int j = 0; j <= (levelSize.y / backgroundTexture.height); j++)
			{
				DrawTextureV(backgroundTexture, { float(i * backgroundTexture.width), float(j * backgroundTexture.height) }, WHITE);
			}
		}
	}
	EndTextureMode();
	staticRenderedBGTexture = staticBGRenderTexture.texture;

	//Tiles Draw
	BeginTextureMode(renderTexture);
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
					(float)tile.getPosition().y,
				};

				//collision rectangles
				if (layer.getName() == "OutdoorSolids")
				{
					Rectangle rec = { (float)tile.getPosition().x * settings::ScreenScale,
									 (float)tile.getPosition().y * settings::ScreenScale,
									 settings::drawSize, settings::drawSize };
					solid_tiles.push_back(std::make_unique<SolidTile>(rec));
					DrawTextureRec(terrainTilesetTexture, source_rect, target_pos, WHITE);
				}
				if (layer.getName() == "LabSolids")
				{
					Rectangle rec = { (float)tile.getPosition().x * settings::ScreenScale,
									 (float)tile.getPosition().y * settings::ScreenScale,
									 settings::drawSize, settings::drawSize };
					solid_tiles.push_back(std::make_unique<SolidTile>(rec));
					DrawTextureRec(laboratoryTilesetTexture, source_rect, target_pos, WHITE);
				}
			}
		}
	}

	EndTextureMode();
	renderedLevelTexture = renderTexture.texture;


	//Decoration Draw
	BeginTextureMode(staticDecorRenderTexture);
	for (auto&& layer : currentLdtkLevel->allLayers())
	{
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

				DrawTextureRec(DecorTex, source_rect, target_pos, WHITE);

			}
		}
	}

	EndTextureMode();
	staticDecorTexture = staticDecorRenderTexture.texture;


	//Paralax Layers Draw
	BeginTextureMode(renderBGTexture);

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
					(float)tile.getPosition().y ,
				};
				DrawTextureRec(bgTilesetTexture, source_rect, target_paralax_pos, WHITE);

			}
		}
	}
	EndTextureMode();
	renderedBGTexture = renderBGTexture.texture;


	//Load entities
	for (auto&& entity : currentLdtkLevel->getLayer("Entities").allEntities())
	{
		if (entity.getName() == "Door")
		{
			Rectangle rect = {(float)entity.getPosition().x * settings::ScreenScale,
							  (float)entity.getPosition().y * settings::ScreenScale,
							  (float)entity.getSize().x * settings::ScreenScale,
							  (float)entity.getSize().y * settings::ScreenScale};
			bool is_right = entity.getField<bool>("Right").value();

			level_entities_safe.push_back(std::make_unique<Door>(rect, is_right));
		}
	}

}

void LevelManager::UnloadLevel()
{

	UnloadTexture(renderedLevelTexture);
	UnloadTexture(renderedBGTexture);
	UnloadTexture(backgroundTexture);
	UnloadTexture(staticDecorTexture);
	UnloadTexture(DecorTex);
	UnloadTexture(bgTilesetTexture);


	UnloadRenderTexture(renderTexture);
	UnloadRenderTexture(renderBGTexture);
	UnloadRenderTexture(staticDecorRenderTexture);
	UnloadRenderTexture(staticBGRenderTexture);
		

	level_entities_safe.clear();
	solid_tiles.clear();


}


void LevelManager::Update(float dt)
{


}

void LevelManager::Draw()
{

	Player& p = *GameScreen::player;
	Camera2D c = GameScreen::camera;


	Vector2 c_position = { (c.offset.x / c.zoom - c.target.x) , (c.offset.y / c.zoom - c.target.y) };

	Vector2 parallaxed = Vector2Multiply(c_position, { 0.05f,0.05f });
	Vector2 parallaxed_far = Vector2Multiply(c_position, { 0.03f,0.03f });


	//Draw static background
	if (currentLdtkLevel->hasBgImage())
	{
		DrawTexturePro(staticRenderedBGTexture,
			{ 0, 0, (float)staticRenderedBGTexture.width, (float)-staticRenderedBGTexture.height },
			{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale },
			{ 0,0 }, 0, WHITE);
	}

	//Draw paralaxed background elements
	DrawTexturePro(renderedBGTexture,
		{ 0, 0, (float)renderedBGTexture.width, (float)-renderedBGTexture.height },//source
		{ parallaxed.x ,parallaxed.y, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale }, //dest
		{ 0,0 }, 0, WHITE);

	//Draw level solids
	DrawTexturePro(renderedLevelTexture,
		{ 0, 0, (float)renderedLevelTexture.width, (float)-renderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale }, 
		{ 0,0 }, 0, WHITE);

	//Draw decorations
	DrawTexturePro(staticDecorTexture,
		{ 0, 0, (float)renderedLevelTexture.width, (float)-renderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale },
		{ 0,0 }, 0, WHITE);
}