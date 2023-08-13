#include <raylib.h>
#include <vector>
#include "Util.h"
#include "GameScreen.h"
#include "Settings.h"
#include "LDtkLoader/Project.hpp"
#include <raymath.h>
#include "rlgl.h"
#include "Dialogue.h"


Shaders* GameScreen::shaders;
std::vector<Entity*> EnitityManager::EntityList;
Camera2D GameScreen::camera;
Camera2D GameScreen::player_focused_cam;
Camera2D GameScreen::shake_cam;
LevelManager* GameScreen::LevelMgr;
SoundManager* GameScreen::SoundMgr;
Player* GameScreen::player;
bool GameScreen::debug = false;
ParticlesManager* GameScreen::Particles;
float GameScreen::shake;
float GameScreen::trauma;



GameScreen::GameScreen()

{
	HideCursor();
	srand(static_cast <unsigned> (time(0)));
	shaders = new Shaders();
	LevelMgr = new LevelManager();
	SoundMgr = new SoundManager();
	player = new Player();
	Particles = new ParticlesManager();
	
	
	//Camera init
	player_focused_cam.target = player->pos();
	player_focused_cam.offset = { settings::screenWidth / 2.0f, settings::screenHeight / 2.0f };
	player_focused_cam.zoom = settings::zoom;
	camera = { 0 };
	camera.target = player->pos();
	camera.offset = { settings::screenWidth / 2.0f, settings::screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = settings::zoom;

	//vignette setup
	Image radialGradient = GenImageGradientRadial(settings::screenHeight*2, settings::screenHeight*2, 0.0f, Fade(BLACK,0.0f), BLACK);
	vignette = LoadTextureFromImage(radialGradient);
	UnloadImage(radialGradient);
}

GameScreen::~GameScreen()
{

}

void GameScreen::UpdateCamera(float dt)
{
	float y_offset = 32.0f;
	// Camera stuff
	// 
	// Camera zoom controls
	player_focused_cam.target = { player->pos().x,player->pos().y - y_offset };
	camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

	if (camera.zoom > 30.0f) camera.zoom = 30.0f;
	else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

	//// Camera reset (zoom and rotation)
	//if (IsKeyPressed(KEY_R))
	//{
	//	camera.zoom = 1.0f;
	//	camera.rotation = 0.0f;
	//}

	static float minSpeed = 1.0f;
	static float minEffectLength = 4.0f;
	static float fractionSpeed = 4.0f;


	camera.offset = { settings::screenWidth / 2.0f, settings::screenHeight / 2.0f };
	Vector2 diff = Vector2Subtract({ player->pos().x,player->pos().y - y_offset }, camera.target);
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

	//Camera shake
	trauma -= dt * 2.4f;
	trauma = trauma < 0.0f ? 0.0f : trauma;

	shake = trauma * trauma;
	
	float temp = float(rand());
	float rand = -1.0f + static_cast <float> (temp) / (static_cast <float> (RAND_MAX / (2.0f)));

	shake_cam.target = camera.target;
	shake_cam.zoom = camera.zoom;
	shake_cam.offset = camera.offset;
	shake_cam.rotation = camera.rotation;
	if (shake > 0.0f)
	{
		shake_cam.rotation = camera.rotation + (max_cam_rotation * shake * rand);
		shake_cam.offset.x = camera.offset.x + (max_cam_x_offset * shake * rand);
		shake_cam.offset.y = camera.offset.y + (max_cam_y_offset * shake * rand);
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
	
	player->Update(dt);
	UpdateCamera(dt);
	LevelMgr->Update(dt);
	EnitityManager::Update(dt);
	DialogueManager::UpdateDialogues(dt);
	Particles->Update(dt);
	shaders->ApplyPerlin();
	return Screens::NONE;
}

void GameScreen::Draw()
{
	Vector2 c_position = { (camera.offset.x / camera.zoom - camera.target.x) , (camera.offset.y / camera.zoom - camera.target.y) };
	BeginMode2D(trauma > 0.0f ? shake_cam : camera);				
	
					// THE HOLY DRAW ORDER: //
	LevelMgr->Draw();					// Level layers (Static Background -> Paralax Background -> Solid tiles -> Level Decorations)
	EnitityManager::Draw(0);			// Entities/Objects behind player
	shaders->DrawPerlin();				// Perlin noise mask
	shaders->DrawOutlines();			// Outline shader
	player->Draw(0);					// Player		
	EnitityManager::Draw(1);			// Entities/Objects in front of player
	LevelMgr->DrawInFrontOfPlayer();	// Decoration Level elements in front of player
	Particles->Draw(1, c_position);		// Particles
	LevelMgr->lights->DrawLightMask();	// Darkness and lights
	LevelMgr->DrawForeGround();			// Paralaxed foreground Level layer
	Particles->Draw(2, c_position);		// Particles foreground
	
	EndMode2D();						// Vignette
	DrawVignette();						// Vignette
	BeginMode2D(trauma > 0.0f ? shake_cam : camera); // Vignette
	 
	player->DrawUI();					// Player UI
	DialogueManager::DrawDialogues();	// Dialogue Boxes


	//DEBUG:
	if (debug)
	{
		////player states
		//std::string stateStr = "" + player->StatesStrMap[player->state];
		//DrawText(stateStr.c_str(), player->center_pos().x, player->center_pos().y -50, 20, BLACK);

		
		//DrawText(std::to_string(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X)).c_str(), player->center_pos().x, player->center_pos().y - 50, 20, BLACK);
		////player animations
		//std::string animStr = "Anim: " + player->animations->m_CurrentActiveAnimation + " :: " +
		//	std::to_string(player->animations->GetAnimation(player->animations->m_CurrentActiveAnimation)->GetCurrentFrameNum());
		//DrawText(animStr.c_str(), player->x, player->y-70, 20, BLACK);
		//player contacts
		//DrawText(player->contact_debug_str.c_str(), player->pos().x, player->pos().y - 100, 1, GREEN);



		DebugShapes();
	}

	EndMode2D();
	std::string hp = "HP: " + std::to_string(player->current_hp) + " / " + std::to_string(player->m_max_hp);
	DrawText(hp.c_str(), 40, 40, 40, GREEN);
	DrawFPS(5, 5);
	//DEBUG:
	if (debug)
	{
		DrawFPS(5, 5);
		std::string txt = "Entity count: " + std::to_string(EnitityManager::EntityList.size());
		DrawText(txt.c_str(), 5, 20, 20, GREEN);
		if (player != nullptr)
		{
			std::string txt2 = "Player POS: " + util::VecToString(player->pos());
			DrawText(txt2.c_str(), 5, 40, 20, GREEN);
			std::string txt3 = "Is touching floor: " + std::to_string(player->is_touching_floor);
			DrawText(txt3.c_str(), 5, 60, 20, GREEN);
		}
	}
	shaders->ResetShaders();			// Have to reset shaders here or nothing else is drawn after shaders...
}

void GameScreen::DebugShapes()
{
	auto world = LevelMgr->world;
	auto currentBody = world->GetBodyList();
	while (currentBody != nullptr)
	{
		auto pos = currentBody->GetPosition();
		DrawCircle(pos.x * settings::PPM,
			pos.y * settings::PPM,
			2,
			PURPLE);
		auto fixture = currentBody->GetFixtureList();
		while (fixture != nullptr)
		{
			auto shape = fixture->GetShape();

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
			if (shape->GetType() == b2Shape::e_circle)
			{
				auto circleShape = (b2CircleShape*)shape;

				auto pos = currentBody->GetPosition();

				DrawCircleLines(pos.x * settings::PPM,
					pos.y * settings::PPM,
					circleShape->m_radius * settings::PPM,
					GREEN);


			}
			else
			{
				auto polygonShape = (b2PolygonShape*)shape;
				int v_count = polygonShape->m_count;

				int vertexCount = polygonShape->m_count;
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

void GameScreen::add_trauma(float intensity)
{
	if (trauma + intensity <= 1.0f)
	{
		trauma += intensity;
	}
	else
	{
		trauma = 1.0f;
	}
	
}

void GameScreen::DrawVignette()
{
	DrawTexturePro(vignette, 
		{ 0,0, (float)vignette.width,(float)vignette.height }, 
		{ 0,0, settings::screenWidth, settings::screenHeight},
		{ 0,0 }, 0.0f, Fade(WHITE, 0.3f));
}





