#include "Dialogue.h"
#include <iostream>
std::unordered_map<int, Dialogue> DialogueManager::DialogueMap;
std::vector<std::unique_ptr<DialogueBox>> DialogueManager::DialogueBoxes;
DialogueState DialogueManager::state = Idle;

void DialogueManager::InitDialogues()
{

	DialogueMap.emplace(1, Dialogue(1, "STOP!", 2));
	DialogueMap.emplace(2, Dialogue(2, "Haven't you heard?\nSecurity breach!", 3));
	DialogueMap.emplace(3, Dialogue(3, "Now, we DO have it\nunder control.", 4));
	DialogueMap.emplace(4, Dialogue(4, "So...\nI guess you can go ahead,\njoin your sciency friends.", 5));
	DialogueMap.emplace(5, Dialogue(5, "If you'll manage to\nbypass our blockade.", 6));
	DialogueMap.emplace(6, Dialogue(6, "I'll watch!", 0));
	DialogueMap.emplace(7, Dialogue(7, "Go on,\nyour friends need you!\nGood luck... hehe...", 0));

}

void DialogueBox::DrawDialogue()
{


	if (current_id != 0)
	{
		DrawRectangleRounded({ pos.x - 5,pos.y - 5, widht_height.x + 10, widht_height.y + 10}
									, 0.1f, 5, Color{0, 0, 0, 200});
		DrawTextPro(GetFontDefault(), TextSubtext(DialogueManager::DialogueMap.at(current_id).text.c_str(), 0, f_counter), pos, { 0,0 }, 0.0f, 6.0f, 1, GREEN);
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
		f_counter = 0.0f;
		current_id = DialogueManager::DialogueMap.at(current_id).next_id;
	}
	else if(current_id == 0)
	{
		state = Exhausted;
		f_counter = 0.0f;
		DialogueManager::EndDialogue();
	}
}
