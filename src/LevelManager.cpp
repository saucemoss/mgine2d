#include "LevelManager.h"
#include "SolidTile.h"
#include "Settings.h"
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/World.hpp>
#include <iostream>
#include <string>



LevelManager::LevelManager()
{

	ldtk::Project* p = new ldtk::Project();
	p->loadFromFile("res\\level\\TestLevel.ldtk");
	ldtkProject = p;

	currentTilesetTexture = LoadTexture("res\\Treasure Hunters\\Palm Tree Island\\Sprites\\Terrain\\Terrain (32x32).png");
	ldtkWorld = &ldtkProject->getWorld();
}

LevelManager::~LevelManager()
{
}

void LevelManager::LoadLevel(std::string level_name)

{
	if (currentLdtkLevel!=nullptr)
	{
		UnloadLevel();
	}

	currentLdtkLevel = &ldtkWorld->getLevel(level_name);
	auto testTileLayerTileset = currentLdtkLevel->getLayer("IntGrid").getTileset();
	auto levelSize = currentLdtkLevel->size;
	renderTexture = LoadRenderTexture(levelSize.x, levelSize.y);

	BeginTextureMode(renderTexture);

	if (currentLdtkLevel->hasBgImage())
	{
		backgroundTexture = LoadTexture("res\\Treasure Hunters\\Palm Tree Island\\Sprites\\Background\\BG Image.png");
		DrawTextureEx(backgroundTexture, { 0, 0 }, 0, 4, WHITE);

	}

	// draw all tileset layers
	for (auto&& layer : currentLdtkLevel->allLayers())
	{
		if (layer.hasTileset())
		{
			int i = 0;
			
			solid_tiles.resize(layer.allTiles().size());

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


				//collision rectangles
				Rectangle rec = {
					(float)tile.getPosition().x * settings::ScreenScale,
					(float)tile.getPosition().y * settings::ScreenScale,
					settings::drawSize, settings::drawSize
				};
				solid_tiles.emplace_back(new SolidTile(rec));

				DrawTextureRec(currentTilesetTexture, source_rect, target_pos, WHITE);
			}
		}
	}


	EndTextureMode();
	renderedLevelTexture = renderTexture.texture;


	//Load entities
	for (auto&& entity : currentLdtkLevel->getLayer("Entities").allEntities())
	{
		level_entities.resize(currentLdtkLevel->getLayer("Entities").allEntities().size());
		if (entity.getName() == "LevelPortal")
		{
			Rectangle rec = {(float)entity.getPosition().x * settings::ScreenScale,
							 (float)entity.getPosition().y * settings::ScreenScale,
							 (float)entity.getSize().x * settings::ScreenScale,
							 (float)entity.getSize().y * settings::ScreenScale};

			std::string target_lvl = entity.getField<std::string>("ToLevelStr").value();
 
			float x = entity.getArrayField<float>("PlayerPos").at(0).value();
			float y = entity.getArrayField<float>("PlayerPos").at(1).value();

			level_entities.emplace_back(new LevelPortal(rec, target_lvl, x, y));
		}
	}

}

void LevelManager::UnloadLevel()
{

	UnloadTexture(renderedLevelTexture);
	UnloadTexture(backgroundTexture);
	UnloadRenderTexture(renderTexture);
	for (SolidTile* s : solid_tiles)
	{
		delete s;
	}
	solid_tiles.clear();
	for (Entity* e : level_entities)
	{
		delete e;
	}
	level_entities.clear();

}

void LevelManager::Draw()
{
	auto levelSize = currentLdtkLevel->size;
	DrawTexturePro(renderedLevelTexture,
		{ 0, 0, (float)renderedLevelTexture.width, (float)-renderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale }, { 0,0 }, 0, WHITE);

}
