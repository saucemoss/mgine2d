#include <assert.h>
#include "Game.h"
#include <raylib.h>
#include "Settings.h"
#include <iostream>
#include "Animations.h"
#include "ScreensManager.h"

RenderTexture2D frameBuffer;

Game::Game(int width, int height, int fps, std::string title)
{
	SetConfigFlags(/*FLAG_VSYNC_HINT ||*/ FLAG_MSAA_4X_HINT);
	assert(!GetWindowHandle());	//If assertion triggers : Window is already opened
	SetTargetFPS(fps);
	InitWindow(width, height, title.c_str());
	frameBuffer = LoadRenderTexture(width, height);
	TextureLoader::LoadTextures();
	ScreensManager::Initialize();
	ScreensManager::SetCurrentScreen(Screens::GAME); //TO BE CHANGED TO TITLE
	
	InitAudioDevice();
	SoundManager::LoadSounds();
	SetMasterVolume(0.5f);
	
}

Game::~Game() noexcept
{
	assert(GetWindowHandle()); //if assertion triggers : Window is already closed
	ScreensManager::Cleanup();
	UnloadRenderTexture(frameBuffer);
	CloseWindow();
}

bool Game::GameShouldClose() const
{
	return WindowShouldClose();
}

void Game::Tick()
{
	float dt = GetFrameTime();
	ScreensManager::Update(dt);
	
	BeginTextureMode(frameBuffer);
	ScreensManager::Draw();
	EndTextureMode();
	ClearBackground(BLACK);
	BeginDrawing();
	// NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
	DrawTexturePro(frameBuffer.texture,
		{ 0, 0, (float)frameBuffer.texture.width, -(float)frameBuffer.texture.height },
		{ 0, 0, settings::screenWidth, settings::screenHeight },
		{ 0, 0 }, 0, WHITE);
	EndDrawing();
	
}



