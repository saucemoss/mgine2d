#include "nlohmann/json.hpp"
#include "Terminal.h"
#include "MovingBlock.h"
#include "Settings.h"
#include "GameScreen.h"
#include "raymath.h"

using json = nlohmann::json;
Terminal::Terminal(const Rectangle& rectangle)
	:
	Collidable(rectangle, b2_kinematicBody, TERMNIAL)
{
	InitAnimations();
	EnitityManager::Add(this);
	m_fixture->SetSensor(true);
	state = TerminalState::Idle;
}

Terminal::~Terminal()
{
	EnitityManager::Remove(this);
}

void Terminal::Draw(int l)
{
	auto spritePosX = center_pos().x;
	auto spritePosY = center_pos().y;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
		{ 0,0 },
		0.0f,
		WHITE);
}

void Terminal::Update(float dt)
{
	SwitchFrames(dt);

	if (player_in_sensor && state != TerminalState::Pass)
	{
		auto spritePosX = center_pos().x ;
		auto spritePosY = center_pos().y ;
		Rectangle source = { spritePosX,spritePosY,settings::tileSize,settings::tileSize };
		GameScreen::shaders->ApplyOutline(*sprite, CurrentFrame(), source, { 0,0 }, 0.0f);
	}

	switch (state)
	{
	case TerminalState::Idle:
		if (player_in_sensor && (IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)))
		{
			SetAnimation("TERM_PASS");
			state = TerminalState::Pass;
			PlaySound(SoundManager::sounds["call_sw"]);
			PlaySound(SoundManager::sounds["pc_work"]);
		}
		break;
	case TerminalState::Pass:
		if (AnimationEnded())
		{
			FreezeFrame("TERM_PASS", 14);
			StopSound(SoundManager::sounds["pc_work"]);
		}

		if (!saved)
		{
			// Load JSON data from the selected slot
			std::ifstream loadFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
			json save_data;
			if (loadFile.is_open()) {
				loadFile >> save_data;
				loadFile.close();
			}
			// Access the specific level's terminals object (e.g., Level_0)
			std::string levelName = GameScreen::LevelMgr->currentLdtkLevel->name;
			json& terminalsObject = save_data["level_data"][levelName]["terminals"];

			terminalsObject[m_ldtkID.str()] = true; // Set the terminal value to true

			// Save the updated JSON data back to the file
			std::ofstream outputFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
			if (outputFile.is_open()) {
				outputFile << save_data.dump(4); // Add indentation for better readability
				outputFile.close();
				std::cout << "Data saved successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to open file for writing." << std::endl;
			}

			saved = true;
		}
		break;
	}

}

void Terminal::InitAnimations()
{
	sprite = TextureLoader::GetTexture("TERMINAL");
	animations->InitializeTerminalAnimations();
	SetAnimation("TERM_IDLE");
}
