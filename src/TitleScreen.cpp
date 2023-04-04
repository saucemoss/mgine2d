#include <string>
#include <raylib.h>
#include "Settings.h"
#include "TitleScreen.h"
#include "Animations.h"

using namespace std;

void TitleScreen::Draw()
{
	ClearBackground(RAYWHITE);

	const int texture_x = settings::screenWidth / 2 - texture->width / 2;
	const int texture_y = settings::screenHeight / 2.5 - texture->height / 2;
	DrawTexture(*texture, texture_x, texture_y, WHITE);

	const string text = "Title Screen";
	const Vector2 text_size = MeasureTextEx(GetFontDefault(), text.c_str(), 0, 15);
	DrawText(text.c_str(), settings::screenWidth / 2 - text_size.x / 2, texture_y + texture->height + text_size.y + 10, 30, BLACK);

	int mouseX = GetMouseX() ;
	int mouseY = GetMouseY() ;

	DrawLine(mouseX, 0, mouseX, settings::screenHeight, SKYBLUE);
	DrawLine(0, mouseY, settings::screenWidth, mouseY, GREEN);

	DrawText("press space to play!", settings::screenWidth/2-25, 500, 10, BLACK);
}

Screens TitleScreen::Update(float dt)
{
	if (IsKeyPressed(KEY_SPACE))
	{
		return Screens::GAME;
	}

	return Screens::NONE;
}

TitleScreen::TitleScreen()
{
	texture = TextureLoader::GetTexture("BIG_Z");
}

TitleScreen::~TitleScreen()
{

}