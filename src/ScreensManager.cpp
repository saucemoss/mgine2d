#include "ScreensManager.h"

int ScreensManager::save_slot_num;
bool ScreensManager::is_new_game;


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
	case MENU:
		ScreensManager::currentScreen = new MenuScreen();
		break;
	case NEW_GAME:
		ScreensManager::currentScreen = new NewGameScreen();
		break;
	case GAME:
		ScreensManager::currentScreen = new GameScreen(is_new_game, save_slot_num);
		break;
	case CONTINUE:
		ScreensManager::currentScreen = new ContinueScreen();
		break;
	case OPTIONS:
		ScreensManager::currentScreen = new OptionsScreen();
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