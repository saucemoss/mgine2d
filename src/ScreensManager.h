#pragma once
#include <iostream>
#include <raylib.h>
#include "TitleScreen.h"
#include "GameScreen.h"
#include "BaseScreen.h"
#include "Screens.h"

class ScreensManager
{
private:
	static BaseScreen* currentScreen;

public:
	static void SetCurrentScreen(Screens screen);
	static void Initialize();

	static void Update(float dt);
	static void Draw();
	static void Cleanup();
};

BaseScreen* ScreensManager::currentScreen = nullptr;

void ScreensManager::Initialize()
{
	ScreensManager::SetCurrentScreen(UNSET);
}

void ScreensManager::SetCurrentScreen(Screens screen)
{
	if (screen == NONE)
	{
		return;
	}

	if (ScreensManager::currentScreen != nullptr)
	{
		delete ScreensManager::currentScreen;
		ScreensManager::currentScreen = nullptr;
	}

	switch (screen)
	{
	case UNSET:
		ScreensManager::currentScreen = nullptr;
		break;
	case TITLE:
		ScreensManager::currentScreen = new TitleScreen();
		break;
	case GAME:
		ScreensManager::currentScreen = new GameScreen();
		break;
	case NONE:
		std::cout << "Landed in NONE case for switch. This should never happen!" << std::endl;
		exit(1);
	}
}

void ScreensManager::Update(float dt)
{
	if (ScreensManager::currentScreen != nullptr)
	{
		Screens result = ScreensManager::currentScreen->Update(dt);
		if (result != NONE)
		{
			ScreensManager::SetCurrentScreen(result);
		}
	}
}

void ScreensManager::Draw()
{
	if (ScreensManager::currentScreen != nullptr)
	{
		ScreensManager::currentScreen->Draw();
	}
}

void ScreensManager::Cleanup()
{
	if (ScreensManager::currentScreen != nullptr)
	{
		delete ScreensManager::currentScreen;
		ScreensManager::currentScreen = nullptr;
	}
}