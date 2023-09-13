#include "ContinueScreen.h"
#include "ScreensManager.h"

ContinueScreen::ContinueScreen() : selectedSlot(0)
{
    for (int i = 0; i < 3; ++i)
    {
        slotRectangles[i] = { settings::screenWidth / 2 - 100, (float)150 + i * 100, 200, 100 };
        saveFileExists[i] = FileExists(("save_slot_" + std::to_string(i + 1) + ".json").c_str());
    }
}

void ContinueScreen::Draw()
{
    ClearBackground(BLACK);

    const char title[] = "Continue";
    DrawText(title, settings::screenWidth / 2 - MeasureText(title, 20) / 2, 100, 20, DARKGREEN);

    for (int i = 0; i < 3; ++i)
    {
        DrawRectangleRec(slotRectangles[i], (i == selectedSlot) ? GREEN: DARKGREEN);
        DrawRectangleLines(slotRectangles[i].x, slotRectangles[i].y, slotRectangles[i].width, slotRectangles[i].height, GREEN);
        DrawText(("Slot " + std::to_string(i + 1)).c_str(), slotRectangles[i].x + 10, slotRectangles[i].y + 10, 20, BLACK);
        DrawText(saveFileExists[i] ? "Occupied" : "Empty", slotRectangles[i].x + 10, slotRectangles[i].y + 30, 16, saveFileExists[i] ? RED : RAYWHITE);
    }
    const char back[] = "(B) - Back";
    DrawText(back, settings::screenWidth / 2 - MeasureText(back, 20) / 2, settings::screenHeight / 2, 20, DARKGREEN);
}

Screens ContinueScreen::Update(float dt)
{
    if (IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
    {
        selectedSlot = (selectedSlot - 1 + 3) % 3;
    }
    else if (IsKeyPressed(KEY_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
    {
        selectedSlot = (selectedSlot + 1) % 3;
    }
    
    if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
    {
        if (saveFileExists[selectedSlot])
        {
            ScreensManager::save_slot_num = selectedSlot;
            ScreensManager::is_new_game = false;
            return Screens::GAME; 
        }
    }
    else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
    {
        return Screens::MENU; 
    }

    return Screens::NONE;
}
