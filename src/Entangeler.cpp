#include "nlohmann/json.hpp"
#include "Entangeler.h"
#include "MovingBlock.h"
#include "Settings.h"
#include "GameScreen.h"
#include "raymath.h"
#include "Dialogue.h"
#include "Terminal.h"

using json = nlohmann::json;
Entangeler::Entangeler(const Rectangle& rectangle)
	:
	Collidable(rectangle, b2_kinematicBody, ENTANGELER)
{
	InitAnimations();
	EnitityManager::Add(this);
	m_fixture->SetSensor(true);
	state = EntangelerState::Idle;

	terminals.push_back("6c12e900-3b70-11ee-8449-97688992e95f");
	terminals.push_back("ecd48ad0-3b70-11ee-b323-39c03a1c8cd9");
	terminals.push_back("df3c0330-3b70-11ee-b323-6bcd393fee8f");
	doc_ptr == nullptr;


	std::ifstream loadFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
	json save_data;
	if (loadFile.is_open())
	{
		loadFile >> save_data;
		loadFile.close();
	}
	if (save_data["world_events"]["event1"] == "entangeled_axe")
	{
		state = EntangelerState::Finished;
	}
}

Entangeler::~Entangeler()
{
	EnitityManager::Remove(this);
}

void Entangeler::Draw(int l)
{
	auto spritePosX = center_pos().x - 8.0f;
	auto spritePosY = center_pos().y - 16.0f;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ 0,0 },
		0.0f,
		WHITE);
}

void Entangeler::Update(float dt)
{
	SwitchFrames(dt);

	if (player_in_sensor && state != EntangelerState::Working)
	{
		auto spritePosX = center_pos().x - 8.0f;
		auto spritePosY = center_pos().y - 16.0f;
		Rectangle source = { spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height };
		GameScreen::shaders->ApplyOutline(*sprite, CurrentFrame(), source, { 0,0 }, 0.0f);
	}

	switch (state)
	{
	case EntangelerState::Idle:
		if (player_in_sensor && (IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) && DialogueManager::DialogueExhausted(27))
		{
			state = EntangelerState::Loaded;
			PlaySound(SoundManager::sounds["axe_pickup"]);
			GameScreen::player->m_has_axe = false;

			if (doc_ptr == nullptr)
			{
				for (Entity* e : EnitityManager::EntityList)
				{
					if (e->m_ldtkID.str() == "6f2887c0-3b70-11ee-9f44-25610bd960b0")
					{
						NPCDoc1* d = static_cast<NPCDoc1*>(e);
						doc_ptr = d;
					}
				}
			}
		}
		break;
	case EntangelerState::Loaded:

		for (Entity* e : EnitityManager::EntityList)
		{
			if (e->m_ldtkID.str() == terminals[0] || e->m_ldtkID.str() == terminals[1] || e->m_ldtkID.str() == terminals[2])
			{
				Terminal* t = static_cast<Terminal*>(e);
				if (t->state == TerminalState::Pass)
				{
					terminal_on_num++;
				}
			}
		}
		if (terminal_on_num == 3)
		{
			terminals_on = true;
		}
		else
		{
			terminal_on_num = 0;
		}

		if (terminals_on && DialogueManager::DialogueExhausted(47))
		{
			SetAnimation("ENTANG");
			state = EntangelerState::Working;
			PlaySound(SoundManager::sounds["call_sw"]);
			PlaySound(SoundManager::sounds["pc_work"]);
		}

		break;
	case EntangelerState::Working:
		if (AnimationEnded())
		{
			FreezeFrame("ENTANG", 1);
			StopSound(SoundManager::sounds["pc_work"]);
			state = EntangelerState::Finished;
		}
		break;
	case EntangelerState::Finished:
		if (player_in_sensor && (IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) && 
			!GameScreen::player->m_throwing_unlocked)
		{
			GameScreen::player->m_throwing_unlocked = true;
			GameScreen::player->m_energy_unlocked = true;
			GameScreen::player->m_has_axe = true;
			PlaySound(SoundManager::sounds["axe_pickup"]);
			std::ifstream loadFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
			json save_data;
			if (loadFile.is_open())
			{
				loadFile >> save_data;
				loadFile.close();
			}

			save_data["player"]["skillsUnlocked"]["axe_throwing"] = true;
			save_data["world_events"]["event1"] = "entangeled_axe";
			//Used Dialogue Lines
			json usedLinesArray = json::array();
			for (int line : DialogueManager::used_lines) {
				usedLinesArray.push_back(line);
			}
			save_data["dialogue_used_lines"] = usedLinesArray;

			// Save the updated JSON data back to the file
			std::ofstream outputFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
			if (outputFile.is_open()) {
				outputFile << save_data;
				outputFile.close();
				std::cout << "Data saved successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to open file for writing." << std::endl;
			}
		}
		break;
	}

}

void Entangeler::InitAnimations()
{
	sprite = TextureLoader::GetTexture("ENTANG");
	animations->InitializeEntanglerAnimations();
	FreezeFrame("ENTANG",1);
}
