#include "nlohmann/json.hpp"
#include "MediPod.h"
#include "LevelManager.h"
#include "SoundManager.h"
#include "GameScreen.h"
#include "Dialogue.h"

using json = nlohmann::json;
MediPod::MediPod(const Rectangle& rect)
	:
	Collidable(rect, b2_staticBody, MPOD)
{
	InitAnimations();
	m_fixture->SetSensor(true);

	state = MediPodState::Closed;

	EnitityManager::Add(this);

}

MediPod::~MediPod()
{
	EnitityManager::Remove(this);
}


void MediPod::Update(float dt)
{
	SwitchFrames(dt);

	if (player_in_sensor && state != MediPodState::Saving)
	{
		Rectangle source = { center_pos().x,center_pos().y - 19.0f,CurrentFrame().width,CurrentFrame().height };
		GameScreen::shaders->ApplyOutline(*sprite, CurrentFrame(), source, { 0,0 }, 0.0f);
	}

	if (!player_in_sensor)
	{
		saved = false;
	}

	switch (state)
	{
	case MediPodState::Open:
		//StopSound(my_engine_sound);
		if (!player_in_sensor)
		{
			state = MediPodState::Closing;
			PlayOnce("MPOD_CLOSE");
		}
		else
		{
			if ((IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) && !saved)
			{
				PlayOnce("MPOD_CLOSE");
				state = MediPodState::Saving;
			}
		}
		break;
	case MediPodState::Closed:
		//StopSound(my_engine_sound);
		if (!player_in_sensor)
		{

		}
		else
		{
			state = MediPodState::Opening;
			PlayOnce("MPOD_OPEN");
		}
		break;
	case MediPodState::Closing:
		if (!player_in_sensor)
		{
			//if (!IsSoundPlaying(my_engine_sound)) PlaySound(my_engine_sound);

			if (AnimationEnded())
			{
				state = MediPodState::Closed;
				PlaySound(SoundManager::sounds["door_close"]);
				GameScreen::add_trauma(0.5f);
				ParticleEmitter* p = new ParticleEmitter(pos());
				GameScreen::Particles->Add(DefinedEmitter::dust, p);
				p->EmitParticles();
			}
		}
		else
		{
			state = MediPodState::Opening;
			//transfer current anim frame num to new anim
			int frameNum = 7 - animation->GetCurrentFrameNum();
			PlayFromFrame(frameNum, "MPOD_OPEN");
			animation->PlayOnce();
		}
		break;
	case MediPodState::Opening:
		if (!player_in_sensor)
		{
			state = MediPodState::Closing;
			//transfer current anim frame num to new anim
			int frameNum = 7 - animation->GetCurrentFrameNum();
			PlayFromFrame(frameNum, "MPOD_CLOSE");
			animation->PlayOnce();
		}
		else
		{
			//if (!IsSoundPlaying(my_engine_sound)) PlaySound(my_engine_sound);
			if (AnimationEnded())
			{
				state = MediPodState::Open;
				PlaySound(SoundManager::sounds["door_close"]);
			}
		}
		break;
	case MediPodState::Saving:
		GameScreen::player->state = PlayerState::MediPodSave;

		if (animations->m_CurrentActiveAnimation == "MPOD_CLOSE"  && AnimationEnded())
		{
			PlayOnce("MPOD_SAVE");
		}
		if (animations->m_CurrentActiveAnimation == "MPOD_SAVE" && AnimationEnded())
		{
			SaveGame();
			saved = true;

			PlayOnce("MPOD_OPEN");
		}
		if (animations->m_CurrentActiveAnimation == "MPOD_OPEN" && AnimationEnded())
		{
			state = MediPodState::Open;
			GameScreen::player->state = PlayerState::Idle;
			GameScreen::player->visible = true;
			GameScreen::player->current_hp = GameScreen::player->m_max_hp;
			DialogueManager::StartDialogue(10, center_pos());
		}

		break;
	}
}

void MediPod::Draw(int l)
{
	auto spritePosX = center_pos().x;
	auto spritePosY = center_pos().y - 19.0f;


	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ 0,0 },
		0.0f,
		WHITE);

}

void MediPod::InitAnimations()
{
	sprite = TextureLoader::GetTexture("MPOD");
	animations->InitializeMediPodAnimations();
	FreezeFrame("MPOD_CLOSE", 7);
}

void MediPod::SaveGame()
{
	std::ifstream loadFile("save_slot_" + std::to_string(GameScreen::LevelMgr->save_file_num + 1) + ".json");
	json save_data;
	if (loadFile.is_open())
	{
		loadFile >> save_data;
		loadFile.close();
	}

	// Update the current level and player position
	save_data["player"]["currentLevel"] = GameScreen::LevelMgr->currentLdtkLevel->name;
	save_data["player"]["position"] = { GameScreen::player->pos().x, GameScreen::player->pos().y};

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