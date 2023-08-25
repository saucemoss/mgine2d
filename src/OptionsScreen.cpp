#include "OptionsScreen.h"
#include "ScreensManager.h"
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_NO_ICONS
#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS

#include "raygui\raygui.h"    
OptionsScreen::OptionsScreen()
{

}

void OptionsScreen::Draw()
{
    ClearBackground(BLACK);

    const char title[] = "Options - unimplemented";
    DrawText(title, settings::screenWidth / 2 - MeasureText(title, 20) / 2, 100, 20, DARKGREEN);


    GuiCheckBox( { float(settings::screenWidth / 2 - MeasureText("Option", 20) / 2), 320, 20, 20 }, "Option", &options_option);

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
