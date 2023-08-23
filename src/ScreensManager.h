#ifndef SCREENMNG_H
#define SCREENMNG_H

#pragma once
#include <iostream>
#include <raylib.h>
#include "OptionsScreen.h"
#include "ContinueScreen.h"
#include "NewGameScreen.h"
#include "TitleScreen.h"
#include "GameScreen.h"
#include "BaseScreen.h"
#include "MenuScreen.h"
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

	static int save_slot_num;
	static bool is_new_game;
};

#endif