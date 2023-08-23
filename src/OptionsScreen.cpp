#include "OptionsScreen.h"
#include "ScreensManager.h"

OptionsScreen::OptionsScreen()
{

}

void OptionsScreen::Draw()
{
    ClearBackground(BLACK);

    const char title[] = "Options - unimplemented";
    DrawText(title, settings::screenWidth / 2 - MeasureText(title, 20) / 2, 100, 20, DARKGREEN);

    const char back[] = "(B) - Back";
    DrawText(back, settings::screenWidth / 2 - MeasureText(back, 20) / 2, settings::screenHeight / 2, 20, DARKGREEN);
}

Screens OptionsScreen::Update(float dt)
{
    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
    {
        return Screens::MENU;
    }

    return Screens::NONE;
}
