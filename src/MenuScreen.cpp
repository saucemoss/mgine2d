#include "MenuScreen.h"
#include "Settings.h"
#include "Animations.h"
#include "Game.h"

MenuScreen::MenuScreen() : selectedOption(MenuOption::NewGame), optionIndex(0) {}

void MenuScreen::Draw()
{
    ClearBackground(BLACK);

    // Draw the menu options
    const char title[] = "Project: Station Aurora";
    DrawText(title, settings::screenWidth / 2 - MeasureText(title, 20) / 2, 100, 20, DARKGREEN);

    const char* options[] = { "Continue", "New Game", "Options", "Exit" };
    for (int i = 0; i < 4; ++i)
    {
        Color color = (i == optionIndex) ? GREEN : DARKGREEN;
        DrawText(options[i], settings::screenWidth / 2 - MeasureText(options[i], 20) / 2, 200 + i * 50, 20, color);
    }
}

Screens MenuScreen::Update(float dt)
{
    // Handle input to navigate through menu options
    if (IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
    {
        optionIndex = (optionIndex - 1 + 4) % 4;
    }
    else if (IsKeyPressed(KEY_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
    {
        optionIndex = (optionIndex + 1) % 4;
    }

    selectedOption = MenuOption(optionIndex);

    if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
    {
        // Handle the selected option
        switch (selectedOption)
        {
        case MenuOption::Continue:
            return Screens::CONTINUE; 
        case MenuOption::NewGame:
            return Screens::NEW_GAME; 
        case MenuOption::Options:
            return Screens::OPTIONS; 
        case MenuOption::Exit:
            std::cout << "exit" << std::endl;
            Game::exit_window = true;
            break;
        }
    }

    return Screens::NONE;
}
