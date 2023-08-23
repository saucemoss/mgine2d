#include "nlohmann/json.hpp"
#include "Switch.h"
#include "Settings.h"
#include "GameScreen.h"
#include "raymath.h"
#include "Elevator.h"
using json = nlohmann::json;

Switch::Switch(const Rectangle& rectangle)
	:
	Collidable(rectangle, b2_kinematicBody, SWITCH)
{
	InitAnimations();
	EnitityManager::Add(this);
	m_fixture->SetSensor(true);
	state = SwitchState::Off;
}

Switch::~Switch()
{
	EnitityManager::Remove(this);
}

void Switch::Draw(int l)
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

void Switch::Update(float dt)
{
	SwitchFrames(dt);


	switch (state)
	{
	case SwitchState::Off:
		FreezeFrame("SWITCH", 1);
		if (axed)
		{
			state = SwitchState::On;
			PlaySound(SoundManager::sounds["electroswitch-01"]);
			//PlaySound(SoundManager::sounds["pc_work"]);

		}
		break;
	case SwitchState::On:

		FreezeFrame("SWITCH", 2);
		if(!IsSoundPlaying(SoundManager::sounds["electroswitch-01"]) && !IsSoundPlaying(SoundManager::sounds["electroswitch-02"]))
			PlaySound(SoundManager::sounds["electroswitch-02"]);

		for (Entity* e : EnitityManager::EntityList)
		{
			if (e->m_ldtkID == m_elevator_reference)
			{
				Elevator* elevator = dynamic_cast<Elevator*>(e);
				elevator->powered = true;
			}
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
			// Access the specific level's  object (e.g., Level_0)
			std::string levelName = GameScreen::LevelMgr->currentLdtkLevel->name;
			json& obj = save_data["level_data"][levelName]["switches"];

			obj[m_ldtkID.str()] = true; // Set the value to true

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

void Switch::InitAnimations()
{
	sprite = TextureLoader::GetTexture("SWITCH");
	animations->InitializeSwitchAnimations();
	FreezeFrame("SWITCH", 1);
}
