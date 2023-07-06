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
#include <rlgl.h>
#include "MovingBlock.h"
#include "WoodCrate.h"
#include "Platform.h"
#include "InfectedHazmat.h"
#include "FireAxe.h"
#include "ContactListener.h"

b2World* LevelManager::world = nullptr;
b2World* Collidable::world = nullptr;
std::vector<std::unique_ptr<Entity>> LevelManager::level_entities_safe;


// Move a light and mark it as dirty so that we update it's mask next frame
void LevelManager::MoveLight(LightInfo& light, float x, float y)
{
	light.dirty = true;
	light.position.x = x;
	light.position.y = y;

	// update the cached bounds
	light.bounds.x = x - light.outerRadius ;
	light.bounds.y = y - light.outerRadius ;

}

// Compute a shadow volume for the edge
// It takes the edge and projects it back by the light radius and turns it into a quad
void LevelManager::ComputeShadowVolumeForEdge(LightInfo& light, Vector2 sp, Vector2 ep)
{
	if (light.shadowCount >= MAX_SHADOWS) return;

	float extension = light.outerRadius * 2;

	Vector2 spVector = Vector2Normalize(Vector2Subtract(sp, light.position));
	Vector2 spProjection = Vector2Add(sp, Vector2Scale(spVector, extension));

	Vector2 epVector = Vector2Normalize(Vector2Subtract(ep, light.position));
	Vector2 epProjection = Vector2Add(ep, Vector2Scale(epVector, extension));

	light.shadows[light.shadowCount].vertices[0] = sp;
	light.shadows[light.shadowCount].vertices[1] = ep;
	light.shadows[light.shadowCount].vertices[2] = epProjection;
	light.shadows[light.shadowCount].vertices[3] = spProjection;

	light.shadowCount++;
}

// Draw the light and shadows to the mask for a light
void LevelManager::DrawLightMask(LightInfo& light)
{
	// Use the light mask
	BeginTextureMode(light.mask);

	ClearBackground(WHITE);
	// Force the blend mode to only set the alpha of the destination
	rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
	rlSetBlendMode(BLEND_CUSTOM);

	// If we are valid, then draw the light radius to the alpha mask
	if (light.valid) DrawCircleGradient((int)light.position.x, (int)light.position.y, light.outerRadius, ColorAlpha(WHITE, 0), WHITE); 

	rlDrawRenderBatchActive();
	
	// Cut out the shadows from the light radius by forcing the alpha to maximum
	rlSetBlendMode(BLEND_ALPHA);
	rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MAX);
	rlSetBlendMode(BLEND_CUSTOM);

	// Draw the shadows to the alpha mask
	for (int i = 0; i < light.shadowCount; i++)
	{
		DrawTriangleFan(light.shadows[i].vertices, 4, WHITE);
	}

	rlDrawRenderBatchActive();

	// Go back to normal blend mode
	rlSetBlendMode(BLEND_ALPHA);
	
	DrawCircleGradient((int)light.position.x, (int)light.position.y, light.outerRadius, ColorAlpha(WHITE,0), WHITE);
	
	EndTextureMode();
}

void LevelManager::RemoveEntityFromLevel(Entity& e)
{
	e.m_destroy = true;
}

bool LevelManager::UpdateLight(LightInfo& light, std::vector<Rectangle*> m_light_walls)
{
	if (!light.active || !light.dirty) return false;

	light.dirty = false;
	light.shadowCount = 0;
	light.valid = false;

	for (int i = 0; i < m_light_walls.size(); i++)
	{
		// Are we in a box? if so we are not valid
		if (CheckCollisionPointRec(light.position, *m_light_walls[i])) return false;

		// If this box is outside our bounds, we can skip it
		if (!CheckCollisionRecs(light.bounds, *m_light_walls[i])) continue;

		// Check the edges that are on the same side we are, and cast shadow volumes out from them

		// Top
		Vector2 sp = { m_light_walls[i]->x, m_light_walls[i]->y };
		Vector2 ep = { m_light_walls[i]->x + m_light_walls[i]->width, m_light_walls[i]->y };

		if (light.position.y > ep.y) ComputeShadowVolumeForEdge(light, sp, ep);

		// Right
		sp = ep;
		ep.y += m_light_walls[i]->height;
		if (light.position.x < ep.x) ComputeShadowVolumeForEdge(light, sp, ep);

		// Bottom
		sp = ep;
		ep.x -= m_light_walls[i]->width;
		if (light.position.y < ep.y) ComputeShadowVolumeForEdge(light, sp, ep);

		// Left
		sp = ep;
		ep.y -= m_light_walls[i]->height;
		if (light.position.x > ep.x) ComputeShadowVolumeForEdge(light, sp, ep);

		// The box itself
		light.shadows[light.shadowCount].vertices[0] = { m_light_walls[i]->x, m_light_walls[i]->y };
		light.shadows[light.shadowCount].vertices[1] = { m_light_walls[i]->x, m_light_walls[i]->y + m_light_walls[i]->height };
		light.shadows[light.shadowCount].vertices[2] = { m_light_walls[i]->x + m_light_walls[i]->width, m_light_walls[i]->y + m_light_walls[i]->height };
		light.shadows[light.shadowCount].vertices[3] = { m_light_walls[i]->x + m_light_walls[i]->width, m_light_walls[i]->y };
		light.shadowCount++;
	}

	light.valid = true;

	DrawLightMask(light);

	return true;
};

void LevelManager::SetupLight(float x, float y, float radius)
{
	m_lights.push_back(LightInfo());
	m_lights.back().active = true;
	m_lights.back().valid = false;  // The light must prove it is valid
	m_lights.back().mask = LoadRenderTexture(currentLdtkLevel->size.x, currentLdtkLevel->size.y);
	m_lights.back().outerRadius = radius;

	m_lights.back().bounds.width = radius * 2;
	m_lights.back().bounds.height = radius * 2;

	MoveLight(m_lights.back(), x, y);

	// Force the render texture to have something in it
	DrawLightMask(m_lights.back());
}

void LevelManager::SetupBoxes()
{

	//auto currentBody = world->GetBodyList();
	//while (currentBody != nullptr)
	//{
	//	auto pos = currentBody->GetPosition();
	//	auto fixture = currentBody->GetFixtureList();
	//	while (fixture != nullptr)
	//	{
	//		auto shape = fixture->GetShape();
	//		// Note, right now supposing all shapes are polygons, use to determine shape->GetType();

	//			auto polygonShape = (b2PolygonShape*)shape;
	//			
	//			Rectangle r = {
	//				pos.x * settings::PPM, pos.y * settings::PPM ,
	//				32,32};
	//			m_light_walls.push_back(&r);

	//		fixture = fixture->GetNext();
	//	}

	//	currentBody = currentBody->GetNext();
	//}

	for (auto& s : solid_tiles)
	{
		m_light_walls.push_back(&s->m_rectangle);
	}
	for (auto& e : level_entities_safe)
	{
		Collidable* c = dynamic_cast<Collidable*>(e.get());
		if (c != nullptr)
		{
			if (c->m_colliderTag == M_BLOCK)
			{
				m_light_walls.push_back(&c->m_rectangle);
			}
		}
	}
}

LevelManager::LevelManager()
{

	ldtk::Project* p = new ldtk::Project();
	p->loadFromFile("res\\level\\TestLevel.ldtk");
	ldtkProject = p;

	laboratorySolidsSpriteAtlas = LoadTexture("res\\level\\lab2.png");
	ldtkWorld = &ldtkProject->getWorld();
	contacts = new ContactListener();
	world = new b2World(gravity);
	world->SetContactListener(contacts);
	Collidable::world = world;
	LoadLevel("Level_0");
	m_darkness_strength = 0.95f;

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

	if (world == nullptr)
	{
		contacts = new ContactListener();
		world = new b2World(gravity);
		world->SetContactListener(contacts);
	}
	Collidable::world = world;

	if (GameScreen::player != nullptr)
	{
		GameScreen::player->RecreateBody();
		GameScreen::player->NewBody();
	}


	SetupLight(0, 0, 100);
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
	paralaxedBackgroundSpriteAtlas = LoadTexture("res//level//mothman.png");
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
		Rectangle rect = {	(float)entity.getPosition().x,
							(float)entity.getPosition().y,
							(float)entity.getSize().x ,
							(float)entity.getSize().y  };

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
		if (entity.getName() == "Light64")
		{
			float radius = entity.getField<float>("Radius").value();
			SetupLight(rect.x + rect.width/2, rect.y + rect.height/2, radius);
			SetupLight(2*rect.x + rect.width / 2, rect.y + rect.height / 2, radius);
		}
		if (entity.getName() == "Light32")
		{
			float radius = entity.getField<float>("Radius").value();
			SetupLight(rect.x + rect.width / 2, rect.y + rect.height / 2, radius);
		}
		if (entity.getName() == "MovingBlock")
		{
			level_entities_safe.push_back(std::make_unique<MovingBlock>(rect, entity.getArrayField<ldtk::IntPoint>("Path")));
		}
		if (entity.getName() == "WoodCrate")
		{
			level_entities_safe.push_back(std::make_unique<WoodCrate>(rect));
		}
		if (entity.getName() == "PlatformBox")
		{
			level_entities_safe.push_back(std::make_unique<Platform>(rect));
		}
		if (entity.getName() == "InfectedHazmat")
		{
			level_entities_safe.push_back(std::make_unique<InfectedHazmat>(rect));
			level_entities_safe.back().get()->m_draw_layer = 1;
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
	SetupBoxes();

	std::cout << m_lights.size() << std::endl;
	// Create a global light mask to hold all the blended m_lights
	lightMask = LoadRenderTexture(
		currentLdtkLevel->size.x,
		currentLdtkLevel->size.y);
}

void LevelManager::UnloadLevel()
{

	if (world != nullptr)
	{
		// if we had an old world then delete it and recreate
		// a new one for the new level
		delete contacts;
		contacts = nullptr;
		delete world;
		world = nullptr;
	}


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

	const float timeStep = dt;
	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;
	world->Step(timeStep, velocityIterations, positionIterations);

	// Drag light 0

	Vector2 mp = GetScreenToWorld2D(GetMousePosition(), GameScreen::camera);
	Vector2 pp = GetScreenToWorld2D(GameScreen::player_focused_cam.target, GameScreen::player_focused_cam);

	MoveLight(m_lights.at(0), GameScreen::player_focused_cam.target.x, GameScreen::player_focused_cam.target.y);

	for (int i = 1; i < m_lights.size(); i++)
	{
		MoveLight(m_lights.at(i), m_lights.at(i).position.x, m_lights.at(i).position.y);
	}


	// Make a new light
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
	{

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
		if (UpdateLight(m_lights.at(i), m_light_walls)) dirtyLights = true;
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
				(float)GameScreen::LevelMgr->currentLdtkLevel->size.x ,
				-(float)GameScreen::LevelMgr->currentLdtkLevel->size.y }, Vector2Zero(), WHITE);
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
		Vector2Zero(), ColorAlpha(WHITE, m_darkness_strength));

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

bool LevelManager::CheckPlayerInSensor(b2Fixture& sensor)
{
	bool player_in_sensor = false;
	if (sensor.GetBody()->GetContactList() != nullptr)
	{
		auto con = sensor.GetBody()->GetContactList()->contact;
		while (con != nullptr)
		{
			Collidable* playerPtr = nullptr;
			if (con->GetFixtureA() == &sensor)
			{
				playerPtr = reinterpret_cast<Collidable*>(con->GetFixtureB()->GetBody()->GetUserData().pointer);
			}
			else if (con->GetFixtureB() == &sensor)
			{
				playerPtr = reinterpret_cast<Collidable*>(con->GetFixtureA()->GetBody()->GetUserData().pointer);
			}

			if (playerPtr != nullptr && playerPtr->m_colliderTag == PLAYER && con->IsTouching())
			{
				return true;
			}
			else
			{
				con = con->GetNext();
			}	
		}
	}
	return player_in_sensor;
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
	DrawTexturePro(paralaxedBackgroundRenderedLevelTexture,
		{ 0, 0, (float)paralaxedBackgroundRenderedLevelTexture.width, (float)-paralaxedBackgroundRenderedLevelTexture.height },//source
		{ parallaxed.x ,parallaxed.y, (float)levelSize.x ,(float)levelSize.y }, //dest
		{ 0,0 }, 0, WHITE);
}

void LevelManager::Draw()
{

	Player& p = *GameScreen::player;
	Camera2D c = GameScreen::camera;


	Vector2 c_position = { (c.offset.x / c.zoom - c.target.x) , (c.offset.y / c.zoom - c.target.y) };

	Vector2 parallaxed = Vector2Multiply(c_position, { 0.05f,0.05f });
	Vector2 parallaxed_far = Vector2Multiply(c_position, { 0.01f,0.01f });


	////Draw static background
	if (currentLdtkLevel->hasBgImage())
	{
		DrawTexturePro(baseBackgroundRenderedLevelTexture,
			{ 0, 0, (float)baseBackgroundRenderedLevelTexture.width, (float)-baseBackgroundRenderedLevelTexture.height },
			{ 0, 0, (float)levelSize.x ,(float)levelSize.y  },
			{ 0,0 }, 0, WHITE);
	}

	//Draw paralaxed background elements
	//DrawTexturePro(paralaxedBackgroundRenderedLevelTexture,
	//	{ 0, 0, (float)paralaxedBackgroundRenderedLevelTexture.width, (float)-paralaxedBackgroundRenderedLevelTexture.height },//source
	//	{ parallaxed.x ,parallaxed.y, (float)levelSize.x ,(float)levelSize.y  }, //dest
	//	{ 0,0 }, 0, WHITE);

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


