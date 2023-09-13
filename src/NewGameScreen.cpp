#include "NewGameScreen.h"
#include "ScreensManager.h"

NewGameScreen::NewGameScreen() : selectedSlot(0)
{
    for (int i = 0; i < 3; ++i)
    {
        slotRectangles[i] = { settings::screenWidth / 2 - 100, (float)150 + i * 100, 200, 100 };
        saveFileExists[i] = FileExists(("save_slot_" + std::to_string(i + 1) + ".json").c_str());
    }
}

void NewGameScreen::Draw()
{
    ClearBackground(BLACK);

    // Draw the menu options
    const char title[] = "New Game";
    DrawText(title, settings::screenWidth / 2 - MeasureText(title, 20) / 2, 100, 20, DARKGREEN);

    for (int i = 0; i < 3; ++i)
    {
        DrawRectangleRec(slotRectangles[i], (i == selectedSlot) ? GREEN : DARKGREEN);
        DrawRectangleLines(slotRectangles[i].x, slotRectangles[i].y, slotRectangles[i].width, slotRectangles[i].height,  GREEN);
        DrawText(("Slot " + std::to_string(i + 1)).c_str(), slotRectangles[i].x + 10, slotRectangles[i].y + 10, 20, BLACK);
        DrawText(saveFileExists[i] ? "Occupied" : "Empty", slotRectangles[i].x + 10, slotRectangles[i].y + 30, 16, saveFileExists[i] ? RED : RAYWHITE);
    }
    const char back[] = "(B) - Back";
    DrawText(back, settings::screenWidth / 2 - MeasureText(back, 20) / 2, settings::screenHeight / 2, 20, DARKGREEN);
}

Screens NewGameScreen::Update(float dt)
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
        // Delete the existing save file (if it exists)
        if (saveFileExists[selectedSlot])
        {
            remove(("save_slot_" + std::to_string(selectedSlot + 1) + ".json").c_str());
        }

        // Create a new save file with initial data
        json data = {
            { "player", {
                { "position", {0, 0} },
                { "currentLevel", "Level_0" },
                { "maxHP", 50 },
                { "currentHP", 50 },
                { "maxEnergy", 50 },
                { "currentEnergy", 50 },
                { "samples", 0 },
                { "skillsUnlocked", {
                    { "axe", false },
                    { "axe_throwing", false },
                    { "axe_pwr_attack", false },
                    { "axe_dash_slash", false },
                    { "tnt", false }
                } }
            } },
            { "world_events", {
                { "event1", "" },
                { "event2", "" }
            } },
            { "level_data", {
                { "Level_0", {
                    { "visited", true }
                } },
                { "Level_1", {
                    { "visited", false },
                    { "terminals", {
                        { "605df6a0-3b70-11ee-9c52-9bfc6acacb1f", false }
                    } },
                    { "secrets", {
                        { "202b9230-3b70-11ee-9c52-8b1a81cb9d45", false },
                        { "33169870-3b70-11ee-9c52-b138e98d70a5", false }
                    } }
                } },
                { "Level_2", {
                    { "visited", false },
                    { "terminals", {
                        { "24664c50-3b70-11ee-b323-2ffa2017cfd2", false }
                    } }
                } },
                { "Level_3", {
                    { "visited", false },
                    { "terminals", {
                        { "34c233c0-3b70-11ee-892e-2f2967ac2abd", false },
                        { "367b2850-1460-11ee-b55c-79dea731b1fb", false },
                        { "49f19c20-1460-11ee-b55c-978d5783690c", false },
                        { "de41e390-3b70-11ee-af06-f1f029e960ad", false },
                        { "0264a780-1460-11ee-b55c-9fdcedee87c2", false },
                        { "f8604320-1460-11ee-b55c-cb6f31ce9baf", false }
                    } },
                    { "secrets", {
                        { "43272710-3b70-11ee-9502-9b39fa813106", false },
                        { "5b387200-3b70-11ee-b142-ed2d979caaf4", false }
                    } }
                } },
                { "Level_4", {
                    { "visited", false },
                    { "terminals", {
                        { "d69225c0-3b70-11ee-b323-3b405c3d3566", true },
                        { "ecd48ad0-3b70-11ee-b323-39c03a1c8cd9", false },
                        { "df3c0330-3b70-11ee-b323-6bcd393fee8f", false }
                    } },
                    { "switches", {
                        { "747210c0-3b70-11ee-b323-a19bc94561db", false },
                        { "7a4346a0-3b70-11ee-91d6-efedd4476184", false }
                    } }
                } },
                { "Level_5", {
                    { "visited", false },
                    { "switches", {
                        { "3880d9a0-3b70-11ee-b323-99f98253e38c", false }
                    } }
                } },
                { "Level_6", {
                    { "visited", false },
                    { "boss", {
                        { "9c89b140-3b70-11ee-a1ad-4372584a07b6", false }
                    } },
                    { "secrets", {
                        { "4b18dc70-3b70-11ee-9502-63491715186c", false }
                    } }
                } },
                { "Level_7", {
                    { "visited", false },
                    { "switches", {
                        { "490cb700-3b70-11ee-af06-3f57c02f5033", false }
                    } }
                } },
            } },
            { "dialogue_used_lines", json::array() },
            { "game_progress", 0}
        };

        //std::cout << data.dump(4) << std::endl; // Output JSON with indentation

        std::ofstream saveFile("save_slot_" + std::to_string(selectedSlot + 1) + ".json");
        if (saveFile.is_open())
        {
            saveFile << data;
            saveFile.close();
        }
        ScreensManager::save_slot_num = selectedSlot;
        ScreensManager::is_new_game = true;
        return Screens::GAME; // Replace with appropriate enum value
    }
    else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
    {
        return Screens::MENU; // Go back to the menu screen
    }

    return Screens::NONE;
}
