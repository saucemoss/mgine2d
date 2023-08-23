#include <string>
#include <raylib.h>
#include "Settings.h"
#include "TitleScreen.h"
#include "Animations.h"

using namespace std;

void TitleScreen::Draw()
{
	ClearBackground(BLACK);



	DrawRectangle(settings::screenWidth / 2 - 128, settings::screenHeight / 2 - 128, 256, 256, RAYWHITE);
	DrawRectangle(settings::screenWidth / 2 - 112, settings::screenHeight / 2 - 112, 224, 224, BLACK);
	DrawText("raylib", settings::screenWidth / 2 - 44, settings::screenHeight / 2 + 48, 50, RAYWHITE);


	const char title[] = "Title Screen: My Metroidvania Game";
	DrawText(title, settings::screenWidth / 2 - MeasureText(title, 40) / 2, 800, 40, GREEN);
	const char press[] = "Press any key.";
	DrawText(press, settings::screenWidth / 2 - MeasureText(press, 20) / 2, 840, 20, DARKGREEN);
}

Screens TitleScreen::Update(float dt)
{

	if (GetGamepadButtonPressed()!=-1)
	{
		return Screens::MENU;
	}
	 
	return Screens::NONE;
}

TitleScreen::TitleScreen()
{

}

TitleScreen::~TitleScreen()
{

}