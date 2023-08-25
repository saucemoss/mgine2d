#include "nlohmann/json.hpp"
#include "SecretFog.h"
#include "GameScreen.h"
using json = nlohmann::json;
SecretFog::SecretFog(const Rectangle& rectangle)
	:
	Collidable(rectangle, b2_staticBody, SFOG)
{
	m_fixture->SetSensor(true);
	m_rectangle = rectangle;
	EnitityManager::Add(this);
}

SecretFog::~SecretFog()
{
	EnitityManager::Remove(this);

}

void SecretFog::Update(float dt)
{
	if (player_in_sensor) revealed = true;
	if (revealed)
	{
		alpha_reveal -= dt;
		if (!sound_played)
		{
			PlaySound(SoundManager::sounds["secret"]);
			sound_played = true;
		}
	}

	if (alpha_reveal <= 0.0f)
	{
		m_destroy = true;
		// Load JSON data from the selected slot
		std::ifstream loadFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
		json save_data;
		if (loadFile.is_open()) {
			loadFile >> save_data;
			loadFile.close();
		}
		// Access the specific level's terminals object (e.g., Level_0)
		std::string levelName = GameScreen::LevelMgr->currentLdtkLevel->name;
		json& obj = save_data["level_data"][levelName]["secrets"];

		obj[m_ldtkID.str()] = true; // Set the terminal value to true

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
	}
}

void SecretFog::Draw(int l)
{
	DrawRectangleRec(m_rectangle, Fade(Color{ 16, 20, 31, 255 }, alpha_reveal));
}
