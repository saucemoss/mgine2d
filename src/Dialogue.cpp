#include "Dialogue.h"
#include <iostream>
std::unordered_map<int, DialogueLine> DialogueManager::DialogueMap;
std::vector<std::unique_ptr<DialogueBox>> DialogueManager::DialogueBoxes;
std::vector<int> DialogueManager::used_lines;
DialogueState DialogueManager::state = Idle;

void DialogueManager::InitDialogues()
{
	//NPC1 
	DialogueMap.emplace(1, DialogueLine(1, "\n       STOP!", 2));
	DialogueMap.emplace(2, DialogueLine(2, "Haven't you heard?\nSecurity breach!", 3));
	DialogueMap.emplace(3, DialogueLine(3, "Now, we DO have it\nunder control.", 4));
	DialogueMap.emplace(4, DialogueLine(4, "So...\nI guess you can go ahead,\njoin your sciency friends.", 5));
	DialogueMap.emplace(5, DialogueLine(5, "If you'll manage to\nbypass our blockade.", 6));
	DialogueMap.emplace(6, DialogueLine(6, "Go try!\nI'll watch from here.", 0));
	DialogueMap.emplace(7, DialogueLine(7, "Go on,\nyour friends need you!\nGood luck... hehe...", 0));
	
	//Axe pickup statnion
	DialogueMap.emplace(8, DialogueLine(8, "\n       Break in case\n       of emergency.", 0));
	DialogueMap.emplace(9, DialogueLine(9, "\n       RT to Attack", 0));

	//Medipod save
	DialogueMap.emplace(10, DialogueLine(10, "\n       Saved.", 0));
}

void DialogueBox::DrawDialogue()
{


	if (current_id != 0)
	{

		int counter = DialogueManager::DialogueMap.at(current_id).text.size();
		if (f_counter < counter)
		{
			std::string beeps[] = { "beep-01","beep-02","beep-05" };
			if(!IsSoundPlaying(SoundManager::sounds["beep-01"]) && !IsSoundPlaying(SoundManager::sounds["beep-02"]) && !IsSoundPlaying(SoundManager::sounds["beep-05"]))
			SoundManager::PlayRandSounds(beeps, 3);
		}

		DrawRectangleRounded({ pos.x - 5,pos.y - 5, widht_height.x + 10, widht_height.y + 10}
									, 0.1f, 5, Color{0, 0, 0, 200});
		DrawTextPro(GetFontDefault(), TextSubtext(DialogueManager::DialogueMap.at(current_id).text.c_str(), 0, f_counter), pos, { 0,0 }, 0.0f, 6.0f, 1, GREEN);

		DrawTextPro(GetFontDefault(), "Y", {pos.x + widht_height.x, pos.y + widht_height.y - 1 - (int)(f_counter/8) % 4}, {0,0}, 0.0f, 6.0f, 1, GREEN);

		DrawRectangleRoundedLines({ pos.x - 5, pos.y - 5, widht_height.x + 10, widht_height.y + 10 }
									, 0.1f,5,1.5f, DARKGREEN);
		DrawRectangleRoundedLines({ pos.x - 5, pos.y - 5, widht_height.x + 10, widht_height.y + 10 }
		, 0.1f, 5, 0.5f, GREEN);
	}
	
}

void DialogueBox::UpdateDialogue(float dt)
{
	f_counter += dt * 60;
	if ((IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) && current_id != 0)
	{
		if (DialogueManager::DialogueMap.at(current_id).next_id == 0)
		{
			DialogueManager::used_lines.push_back(current_id);
		}
		f_counter = 0.0f;
		current_id = DialogueManager::DialogueMap.at(current_id).next_id;
	}
	else if(current_id == 0)
	{
		
		f_counter = 0.0f;
		DialogueManager::EndDialogue();
	}
}
