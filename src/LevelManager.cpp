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
#include "AnimatedDecor.h"
#include "Elevator.h"
#include "ElevatorCallSwitch.h"
#include <rlgl.h>


// Move a light and mark it as dirty so that we update it's mask next frame
void LevelManager::MoveLight(int slot, float x, float y)
{
	m_lights[slot].dirty = true;
	m_lights[slot].position.x = x;
	m_lights[slot].position.y = y;

	// update the cached bounds
	m_lights[slot].bounds.x = x - m_lights[slot].outerRadius;
	m_lights[slot].bounds.y = y - m_lights[slot].outerRadius;

}

// Compute a shadow volume for the edge
// It takes the edge and projects it back by the light radius and turns it into a quad
void LevelManager::ComputeShadowVolumeForEdge(int slot, Vector2 sp, Vector2 ep)
{
	if (m_lights[slot].shadowCount >= MAX_SHADOWS) return;

	float extension = m_lights[slot].outerRadius * 2;

	Vector2 spVector = Vector2Normalize(Vector2Subtract(sp, m_lights[slot].position));
	Vector2 spProjection = Vector2Add(sp, Vector2Scale(spVector, extension));

	Vector2 epVector = Vector2Normalize(Vector2Subtract(ep, m_lights[slot].position));
	Vector2 epProjection = Vector2Add(ep, Vector2Scale(epVector, extension));

	m_lights[slot].shadows[m_lights[slot].shadowCount].vertices[0] = sp;
	m_lights[slot].shadows[m_lights[slot].shadowCount].vertices[1] = ep;
	m_lights[slot].shadows[m_lights[slot].shadowCount].vertices[2] = epProjection;
	m_lights[slot].shadows[m_lights[slot].shadowCount].vertices[3] = spProjection;

	m_lights[slot].shadowCount++;
}

// Draw the light and shadows to the mask for a light
void LevelManager::DrawLightMask(int slot)
{
	// Use the light mask
	BeginTextureMode(m_lights[slot].mask);

	ClearBackground(WHITE);

	// Force the blend mode to only set the alpha of the destination
	rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
	rlSetBlendMode(BLEND_CUSTOM);

	// If we are valid, then draw the light radius to the alpha mask
	if (m_lights[slot].valid) DrawCircleGradient((int)m_lights[slot].position.x, (int)m_lights[slot].position.y, m_lights[slot].outerRadius, ColorAlpha(WHITE, 0), WHITE);

	rlDrawRenderBatchActive();

	// Cut out the shadows from the light radius by forcing the alpha to maximum
	rlSetBlendMode(BLEND_ALPHA);
	rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MAX);
	rlSetBlendMode(BLEND_CUSTOM);

	// Draw the shadows to the alpha mask
	for (int i = 0; i < m_lights[slot].shadowCount; i++)
	{
		DrawTriangleFan(m_lights[slot].shadows[i].vertices, 4, WHITE);
	}

	rlDrawRenderBatchActive();

	// Go back to normal blend mode
	rlSetBlendMode(BLEND_ALPHA);

	EndTextureMode();
}

// See if a light needs to update it's mask
bool LevelManager::UpdateLight(int slot, std::vector<Rectangle*> m_light_walls, int count)
{
	if (!m_lights[slot].active || !m_lights[slot].dirty) return false;

	m_lights[slot].dirty = false;
	m_lights[slot].shadowCount = 0;
	m_lights[slot].valid = false;

	for (int i = 0; i < count; i++)
	{
		// Are we in a box? if so we are not valid
		if (CheckCollisionPointRec(m_lights[slot].position, *m_light_walls[i])) return false;

		// If this box is outside our bounds, we can skip it
		if (!CheckCollisionRecs(m_lights[slot].bounds, *m_light_walls[i])) continue;

		// Check the edges that are on the same side we are, and cast shadow volumes out from them

		// Top
		Vector2 sp = { m_light_walls[i]->x, m_light_walls[i]->y };
		Vector2 ep = { m_light_walls[i]->x + m_light_walls[i]->width, m_light_walls[i]->y };

		if (m_lights[slot].position.y > ep.y) ComputeShadowVolumeForEdge(slot, sp, ep);

		// Right
		sp = ep;
		ep.y += m_light_walls[i]->height;
		if (m_lights[slot].position.x < ep.x) ComputeShadowVolumeForEdge(slot, sp, ep);

		// Bottom
		sp = ep;
		ep.x -= m_light_walls[i]->width;
		if (m_lights[slot].position.y < ep.y) ComputeShadowVolumeForEdge(slot, sp, ep);

		// Left
		sp = ep;
		ep.y -= m_light_walls[i]->height;
		if (m_lights[slot].position.x > ep.x) ComputeShadowVolumeForEdge(slot, sp, ep);

		// The box itself
		m_lights[slot].shadows[m_lights[slot].shadowCount].vertices[0] = { m_light_walls[i]->x, m_light_walls[i]->y };
		m_lights[slot].shadows[m_lights[slot].shadowCount].vertices[1] = { m_light_walls[i]->x, m_light_walls[i]->y + m_light_walls[i]->height };
		m_lights[slot].shadows[m_lights[slot].shadowCount].vertices[2] = { m_light_walls[i]->x + m_light_walls[i]->width, m_light_walls[i]->y + m_light_walls[i]->height };
		m_lights[slot].shadows[m_lights[slot].shadowCount].vertices[3] = { m_light_walls[i]->x + m_light_walls[i]->width, m_light_walls[i]->y };
		m_lights[slot].shadowCount++;
	}

	m_lights[slot].valid = true;

	DrawLightMask(slot);

	return true;
}

// Setup lights
void LevelManager::SetupLight(int slot, float x, float y, float radius)
{
	m_lights.push_back(LightInfo());
	m_lights[slot].active = true;
	m_lights[slot].valid = false;  // The light must prove it is valid
	m_lights[slot].mask = LoadRenderTexture(currentLdtkLevel->size.x * 2, currentLdtkLevel->size.y * 2);
	m_lights[slot].outerRadius = radius;

	m_lights[slot].bounds.width = radius * 2;
	m_lights[slot].bounds.height = radius * 2;

	MoveLight(slot, x, y);

	// Force the render texture to have something in it
	DrawLightMask(slot);
}
// Set up some m_light_walls
void LevelManager::SetupBoxes()
{
	for (auto& s : solid_tiles)
	{ 
		m_light_walls.push_back(&s->rectangle);
	}
}


LevelManager::LevelManager()
{

	ldtk::Project* p = new ldtk::Project();
	p->loadFromFile("res\\level\\TestLevel.ldtk");
	ldtkProject = p;

	laboratorySolidsSpriteAtlas = LoadTexture("res\\level\\lab2.png");
	ldtkWorld = &ldtkProject->getWorld();
	
	LoadLevel("Level_0");
	
	//Level Portals
	//level portals persist in world cause they connect levels, all other entities are unloaded upon new level entry
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

	//Set to new current level
	currentLdtkLevel = &ldtkWorld->getLevel(level_name);
	levelSize = currentLdtkLevel->size;

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
	//renderBGTexture = LoadRenderTexture(levelSize.x, levelSize.y);
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
					(float)tile.getPosition().y,
				};

				//collision rectangles
				if (layer.getName() == "LabSolids")
				{
					Rectangle rec = { (float)tile.getPosition().x * settings::ScreenScale,
									 (float)tile.getPosition().y * settings::ScreenScale,
									 settings::drawSize, settings::drawSize };
					solid_tiles.push_back(std::make_unique<SolidTile>(rec));
					DrawTextureRec(laboratorySolidsSpriteAtlas, source_rect, target_pos, WHITE);
				}
				if (layer.getName() == "Platforms")
				{
					Rectangle rec = { (float)tile.getPosition().x * settings::ScreenScale,
									 (float)tile.getPosition().y * settings::ScreenScale,
									 tile_size * settings::ScreenScale, tile_size * settings::ScreenScale};
					solid_tiles.push_back(std::make_unique<SolidTile>(rec));
					DrawTextureRec(decorationSpriteAtlas, source_rect, target_pos, WHITE);
				}
			}
		}
	}

	EndTextureMode();
	laboratorySolidsRenderedLevelTexture = laboratorySolidsRenderTexture.texture;


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
					level_entities_safe.push_back(std::make_unique<AnimatedDecor>(tile.tileId));
					level_entities_safe.back().get()->SetPos({ 
						(float)tile.getPosition().x* settings::ScreenScale,
						(float)tile.getPosition().y* settings::ScreenScale, });
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
					(float)tile.getPosition().y ,
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
		Rectangle rect = { (float)entity.getPosition().x * settings::ScreenScale,
				  (float)entity.getPosition().y * settings::ScreenScale,
				  (float)entity.getSize().x * settings::ScreenScale,
				  (float)entity.getSize().y * settings::ScreenScale };
		


		if (entity.getName() == "Door")
		{
			bool is_right = entity.getField<bool>("Right").value();
			level_entities_safe.push_back(std::make_unique<Door>(rect, is_right));
			level_entities_safe.back().get()->m_draw_layer = 1;
		}
		if (entity.getName() == "Elevator")
		{
			level_entities_safe.push_back(std::make_unique<Elevator>(rect, entity.getArrayField<int>("Levels"), entity));
			level_entities_safe.back().get()->m_draw_layer = 1;
			level_entities_safe.back().get()->m_ldtkID = entity.iid;;
		}
		if (entity.getName() == "ElevatorCallSwitch")
		{
			ldtk::IID elev_reference = entity.getField<ldtk::EntityRef>("Entity_ref").value().entity_iid;
			level_entities_safe.push_back(std::make_unique<ElevatorCallSwitch>(rect, elev_reference));
		}
	}

	//Lights setup
	SetupBoxes();

	std::cout << m_light_walls.size() << std::endl;
	// Create a global light mask to hold all the blended m_lights
	lightMask = LoadRenderTexture(
		currentLdtkLevel->size.x * 2,
		currentLdtkLevel->size.y * 2);
	// Setup initial light
	SetupLight(0, 600, 400, 100);


}

void LevelManager::UnloadLevel()
{

	UnloadTexture(laboratorySolidsRenderedLevelTexture);
	UnloadTexture(paralaxedBackgroundRenderedLevelTexture);
	UnloadTexture(baseBackgroundSpriteAtlas);
	UnloadTexture(decorationRenderedLevelTexture);
	UnloadTexture(decorationSpriteAtlas);
	UnloadTexture(paralaxedBackgroundSpriteAtlas);


	UnloadRenderTexture(laboratorySolidsRenderTexture);
	UnloadRenderTexture(decorationRenderTexture);
	UnloadRenderTexture(baseBackgroundRenderTexture);
		
	UnloadRenderTexture(lightMask);
	for (int i = 0; i < m_lights.size(); i++)
	{
		if (m_lights[i].active) UnloadRenderTexture(m_lights[i].mask);
	}
	m_light_walls.clear();
	m_lights.clear();
	nextLight = 0;
	level_entities_safe.clear();
	solid_tiles.clear();


}

void LevelManager::Update(float dt)
{
	// Drag light 0

	Vector2 mp = GetScreenToWorld2D(GetMousePosition(), GameScreen::camera);
	Vector2 pp = GetScreenToWorld2D(GameScreen::player_focused_cam.target, GameScreen::player_focused_cam);

	MoveLight(0, GameScreen::player_focused_cam.target.x+10, GameScreen::player_focused_cam.target.y+24);

	// Make a new light
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && (nextLight < MAX_LIGHTS))
	{
		nextLight++;
		SetupLight(nextLight, mp.x, mp.y, 1000);
		
	}

	// Toggle debug info
	if (IsKeyPressed(KEY_TWO))
	{
		showLines = !showLines;
	}

	// Update the m_lights and keep track if any were dirty so we know if we need to update the master light mask
	bool dirtyLights = false;
	for (int i = 0; i < m_lights.size(); i++)
	{
		if (UpdateLight(i, m_light_walls, m_light_walls.size())) dirtyLights = true;
	}

	// Update the light mask
	if (dirtyLights)
	{
		// Build up the light mask
		BeginTextureMode(lightMask);

		ClearBackground(BLACK);

		// Force the blend mode to only set the alpha of the destination
		rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
		rlSetBlendMode(BLEND_CUSTOM);

		// Merge in all the light masks
		for (int i = 0; i < m_lights.size(); i++)
		{
			if (m_lights[i].active) DrawTextureRec(m_lights[i].mask.texture, { 0, 0,
				(float)GameScreen::LevelMgr->currentLdtkLevel->size.x * 2,
				-(float)GameScreen::LevelMgr->currentLdtkLevel->size.y * 2 }, Vector2Zero(), WHITE);
		}

		rlDrawRenderBatchActive();

		// Go back to normal blend
		rlSetBlendMode(BLEND_ALPHA);
		EndTextureMode();
	}

}

void LevelManager::DrawSpotLights()
{

	// Overlay the shadows from all the m_lights
	DrawTextureRec(lightMask.texture,
		{ 0, 0, (float)lightMask.texture.width,
				-(float)lightMask.texture.height },
		Vector2Zero(), ColorAlpha(WHITE, 0.50f));

	// Draw the m_lights
	//for (int i = 0; i < m_lights.size(); i++)
	//{
	//	if (m_lights[i].active) DrawCircle((int)m_lights[i].position.x, (int)m_lights[i].position.y, 10, (i == 0) ? YELLOW : WHITE);
	//}

	if (showLines)
	{
		for (int s = 0; s < m_lights[0].shadowCount; s++)
		{
			DrawTriangleFan(m_lights[0].shadows[s].vertices, 4, DARKPURPLE);
		}

		for (int b = 0; b < m_light_walls.size(); b++)
		{
			if (CheckCollisionRecs(*m_light_walls[b], m_lights[0].bounds)) DrawRectangleRec(*m_light_walls[b], PURPLE);

			DrawRectangleLines((int)m_light_walls[b]->x, (int)m_light_walls[b]->y, (int)m_light_walls[b]->width, (int)m_light_walls[b]->height, DARKBLUE);
		}
	}
}

void LevelManager::Draw()
{

	Player& p = *GameScreen::player;
	Camera2D c = GameScreen::camera;


	Vector2 c_position = { (c.offset.x / c.zoom - c.target.x) , (c.offset.y / c.zoom - c.target.y) };

	Vector2 parallaxed = Vector2Multiply(c_position, { 0.01f,0.01f });
	Vector2 parallaxed_far = Vector2Multiply(c_position, { 0.01f,0.01f });


	//Draw static background
	if (currentLdtkLevel->hasBgImage())
	{
		DrawTexturePro(baseBackgroundRenderedLevelTexture,
			{ 0, 0, (float)baseBackgroundRenderedLevelTexture.width, (float)-baseBackgroundRenderedLevelTexture.height },
			{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale },
			{ 0,0 }, 0, WHITE);
	}

	//Draw paralaxed background elements
	DrawTexturePro(paralaxedBackgroundRenderedLevelTexture,
		{ 0, 0, (float)paralaxedBackgroundRenderedLevelTexture.width, (float)-paralaxedBackgroundRenderedLevelTexture.height },//source
		{ parallaxed.x ,parallaxed.y, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale }, //dest
		{ 0,0 }, 0, WHITE);
	
	//Draw level solids
	DrawTexturePro(laboratorySolidsRenderedLevelTexture,
		{ 0, 0, (float)laboratorySolidsRenderedLevelTexture.width, (float)-laboratorySolidsRenderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale }, 
		{ 0,0 }, 0, WHITE);

	//Draw decorations
	DrawTexturePro(decorationRenderedLevelTexture,
		{ 0, 0, (float)laboratorySolidsRenderedLevelTexture.width, (float)-laboratorySolidsRenderedLevelTexture.height },
		{ 0, 0, (float)levelSize.x * settings::ScreenScale,(float)levelSize.y * settings::ScreenScale },
		{ 0,0 }, 0, WHITE);
	
}