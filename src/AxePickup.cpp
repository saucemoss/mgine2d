#include "nlohmann/json.hpp"
#include "AxePickup.h"
#include "MovingBlock.h"
#include "Settings.h"
#include "GameScreen.h"
#include "raymath.h"
#include "Dialogue.h"
using json = nlohmann::json;

AxePickup::AxePickup(const Rectangle& rectangle)
	:
	Collidable(rectangle, b2_kinematicBody, AXEPICKUP)
{
	InitAnimations();
	EnitityManager::Add(this);
	m_fixture->SetSensor(true);
	state = AxePickupState::unused;
}

AxePickup::~AxePickup()
{
	EnitityManager::Remove(this);
}

void AxePickup::Draw(int l)
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

void AxePickup::Update(float dt)
{
	SwitchFrames(dt);

	if (player_in_sensor && state != AxePickupState::used)
	{
		auto spritePosX = center_pos().x;
		auto spritePosY = center_pos().y;
		Rectangle source = { spritePosX,spritePosY,settings::tileSize,settings::tileSize };
		GameScreen::shaders->ApplyOutline(*sprite, CurrentFrame(), source, { 0,0 }, 0.0f);
		if (!DialogueManager::DialogueExhausted(8))
		{
			DialogueManager::StartDialogue(8, center_pos());
		}
	}

	switch (state)
	{
	case AxePickupState::unused:
		if (player_in_sensor && (IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)))
		{
			SetAnimation("USED");
			state = AxePickupState::used;
			PlaySound(SoundManager::sounds["glass_shatter"]);
			ParticleEmitter* p = new ParticleEmitter(pos());
			GameScreen::Particles->Add(DefinedEmitter::dust, p);
			p->EmitParticles();p->EmitParticles();
			GameScreen::player->m_has_axe = true;
			GameScreen::player->m_axe_unlocked = true;
			GameScreen::add_trauma(0.9f);

			std::ifstream loadFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
			json save_data;
			if (loadFile.is_open())
			{
				loadFile >> save_data;
				loadFile.close();
			}

			save_data["player"]["skillsUnlocked"]["axe"] = true;

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
	case AxePickupState::used:
		if (!DialogueManager::DialogueExhausted(9))
		{
			DialogueManager::StartDialogue(9, center_pos());
		}
		break;
	}

}

void AxePickup::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeAxePickupAnimations();
	SetAnimation("UNUSED");
}
