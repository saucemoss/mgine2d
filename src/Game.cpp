#include <assert.h>
#include "Game.h"
#include <raylib.h>
#include "Settings.h"
#include <iostream>
#include "Animations.h"
#include "ScreensManager.h"
#include "Dialogue.h"

RenderTexture2D frameBuffer;
bool Game::exit_window = false;


Game::Game(int width, int height, int fps, std::string title)
{
	SetConfigFlags(FLAG_VSYNC_HINT/* || FLAG_MSAA_4X_HINT*/);
	assert(!GetWindowHandle());	//If assertion triggers : Window is already opened
	//SetTargetFPS(fps);
	InitWindow(width, height, title.c_str());
	frameBuffer = LoadRenderTexture(width, height);
	InitAudioDevice();
	TextureLoader::LoadTextures();
	ScreensManager::Initialize();
	ScreensManager::SetCurrentScreen(Screens::TITLE); //TO BE CHANGED TO TITLE
	DialogueManager::InitDialogues();

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

void Game::Tick()
{

	float dt = GetFrameTime();
	ScreensManager::Update(dt);

	ClearBackground(BLANK);
	BeginTextureMode(frameBuffer);
	ScreensManager::Draw();
	EndTextureMode();
	ClearBackground(BLANK);
	BeginDrawing();
	ClearBackground(BLANK);

	DrawTexturePro(frameBuffer.texture,
		{ 0.0f, 0.0f, (float)frameBuffer.texture.width, -(float)frameBuffer.texture.height },
		{ 0.0f, 0.0f, (float)settings::screenWidth, (float)settings::screenHeight },
		{ 0.0f, 0.0f }, 0.0f, WHITE);
	EndDrawing();
	ClearBackground(BLANK);

}



