#include "LevelManager.h"
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

	terrainTilesetTexture = LoadTexture("res\\Treasure Hunters\\Palm Tree Island\\Sprites\\Terrain\\Terrain (32x32).png");
	laboratoryTilesetTexture = LoadTexture("res\\Treasure Hunters\\Palm Tree Island\\Sprites\\Terrain\\lab.png");
	bgTilesetTexture = LoadTexture("res\\Treasure Hunters\\Palm Tree Island\\Sprites\\Background\\Big Clouds.png");

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
	if (currentLdtkLevel!=nullptr)
	{
		UnloadLevel();
	}

	currentLdtkLevel = &ldtkWorld->getLevel(level_name);
	levelSize = currentLdtkLevel->size;
	renderTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	renderBGTexture = LoadRenderTexture(levelSize.x, levelSize.y);
	backgroundTexture = LoadTexture("res\\Treasure Hunters\\Palm Tree Island\\Sprites\\Background\\BG Image.png");

	BeginTextureMode(renderTexture);

	// draw all tileset layers
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
				if (layer.getName() == "Solids")
				{
					Rectangle rec = {(float)tile.getPosition().x * settings::ScreenScale,
									 (float)tile.getPosition().y * settings::ScreenScale,
									 settings::drawSize, settings::drawSize};
					solid_tiles.emplace_back(new SolidTile(rec));
					DrawTextureRec(terrainTilesetTexture, source_rect, target_pos, WHITE);
				}
				if (layer.getName() == "TestTiles")
				{
					Rectangle rec = { (float)tile.getPosition().x * settings::ScreenScale,
									 (float)tile.getPosition().y * settings::ScreenScale,
									 settings::drawSize, settings::drawSize };
					solid_tiles.emplace_back(new SolidTile(rec));
					DrawTextureRec(laboratoryTilesetTexture, source_rect, target_pos, WHITE);
				}

			}
		}
	}

	EndTextureMode();
	renderedLevelTexture = renderTexture.texture;


	//paralax layers?
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
	//level_portals.reserve(currentLdtkLevel->getLayer("Entities").getEntitiesByName("LevelPortal_in").size());

	//level_portals.reserve(currentLdtkLevel->getLayer("Entities").getEntitiesByName("LevelPortal_in").size());
	//for (auto&& entity : currentLdtkLevel->getLayer("Entities").allEntities())
	//{
	//	if (entity.getName() == "LevelPortal_in")
	//	{
	//		Rectangle rect = {(float)entity.getPosition().x * settings::ScreenScale,
	//						  (float)entity.getPosition().y * settings::ScreenScale,
	//						  (float)entity.getSize().x * settings::ScreenScale,
	//						  (float)entity.getSize().y * settings::ScreenScale};

	//		std::string target_lvl = entity.getField<std::string>("ToLevelStr").value();
	//		ldtk::IID portal_out = entity.getField<ldtk::EntityRef>("LevelPortal_out").value().entity_iid;

	//		//level_entities.emplace_back(new LevelPortal(rect, target_lvl, portal_out));
	//		level_portals.emplace_back(new LevelPortal(rect, target_lvl, portal_out));
	//	}
	//}

}

void LevelManager::UnloadLevel()
{

	UnloadTexture(renderedLevelTexture);
	UnloadTexture(renderedBGTexture);
	UnloadTexture(backgroundTexture);
	UnloadRenderTexture(renderTexture);
	UnloadRenderTexture(renderBGTexture);
	for (SolidTile* s : solid_tiles)
	{
		delete s;
	}
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

	if (currentLdtkLevel->hasBgImage())
	{
		DrawTextureEx(backgroundTexture, parallaxed_far, 0, 6, WHITE);
	}

	DrawTexturePro(renderedBGTexture,
		{ 0, 0, (float)renderedBGTexture.width, (float)-renderedBGTexture.height },//source
		{ parallaxed.x ,parallaxed.y, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale }, //dest
		{ 0,0 }, 0, WHITE);

	DrawTexturePro(renderedLevelTexture,
		{ 0, 0, (float)renderedLevelTexture.width, (float)-renderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale }, 
		{ 0,0 }, 0, WHITE);

}