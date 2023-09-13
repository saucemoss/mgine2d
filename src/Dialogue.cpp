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

	//Doc1 Dialogues
	DialogueMap.emplace(11, DialogueLine(11, "\n       Hey you!", 12));
	DialogueMap.emplace(12, DialogueLine(12, "Are you infected? ...\nyou don't look like one.", 13));	
	DialogueMap.emplace(13, DialogueLine(13, "It's not safe here.\nWe are meeting at the\nQuantum Entangeler\nexperiment.", 14));
	DialogueMap.emplace(14, DialogueLine(14, "It's just above this lab.\nBut the whole place is\nsealed.", 15));
	DialogueMap.emplace(15, DialogueLine(15, "And this vent shaft\nis out of reach.", 16));
	DialogueMap.emplace(16, DialogueLine(16, "To get there, you'll\nhave to override door\nseals using termianls.", 17));
	DialogueMap.emplace(17, DialogueLine(17, "You can also try\nto wait things out.", 18));
	DialogueMap.emplace(18, DialogueLine(18, "You would be safe in\nMediPod next room,\n...though...\n", 19));
	DialogueMap.emplace(19, DialogueLine(19, "I don't think help\nis comming anytime soon.", 20));
	DialogueMap.emplace(20, DialogueLine(20, "Meet us upstairs...\nIf you can.", 0));

	//Doc1 welcome
	DialogueMap.emplace(21, DialogueLine(21, "Good!\nYou've made it here!",22));
	DialogueMap.emplace(22, DialogueLine(22, "We were worried\nabout the noises\ndownstairs.", 23));
	DialogueMap.emplace(23, DialogueLine(23, "I hope froggy was\nnot infected...", 24));
	DialogueMap.emplace(24, DialogueLine(24, "He always have been\nthe station mascot.", 0));

	//Doc1 idea
	DialogueMap.emplace(25, DialogueLine(25, "I see you've gotten\nquite handy with\nthat fire axe.", 26));
	DialogueMap.emplace(26, DialogueLine(26, "I got an idea:\nthrow it into the\nQuantum Entangeler.", 27));
	DialogueMap.emplace(27, DialogueLine(27, "along with your\nelectronic watch.", 0));

	//Doc1 help w terminals
	DialogueMap.emplace(28, DialogueLine(28, "Now help me out\nwith the terminals!", 0));

	//Doc1 explain controlls
	DialogueMap.emplace(29, DialogueLine(29, "Perfect!\nNow you can freely\nthrow your axe!", 30));
	DialogueMap.emplace(30, DialogueLine(30, "Since it's entangled\nwith the watch,\nyou'll be able to...", 31));
	DialogueMap.emplace(31, DialogueLine(31, "... retrieve it even\nif it ends up\nsomewhere inconvenient.", 32));
	DialogueMap.emplace(32, DialogueLine(32, "Remember these controls:", 33));
	DialogueMap.emplace(33, DialogueLine(33, "\nHold LT for aim mode.", 34));
	DialogueMap.emplace(34, DialogueLine(34, "\nRight analog:\naim adjust", 35));
	DialogueMap.emplace(35, DialogueLine(35, "\nWhile aiming:\nRT to launch the axe.", 36));
	DialogueMap.emplace(36, DialogueLine(36, "\nHold LT again\nto reclaim the axe.", 37));
	DialogueMap.emplace(37, DialogueLine(37, "Try it out on that\nout of reach\nfuse box.", 0));

	//Doc1 axe lore
	DialogueMap.emplace(38, DialogueLine(38, "Just so you know...\nit's not the same axe.", 39));
	DialogueMap.emplace(39, DialogueLine(39, "It is essentially\nregenerated from\nquantum randomness.", 40));
	DialogueMap.emplace(40, DialogueLine(40, "Your axe will need\npotential charge for\nrespawn to be possible.", 41));
	DialogueMap.emplace(41, DialogueLine(41, "Thats the\nconservation of energy\nrule for you...", 42));
	DialogueMap.emplace(42, DialogueLine(42, "Raise the charge\nthrough kinetic\nenergy conjugation.", 43));
	DialogueMap.emplace(43, DialogueLine(43, "In other words:\nhit stuff to\npower your axe.", 44));
	DialogueMap.emplace(44, DialogueLine(44, "Some energy will\ntrickle in just from\nquantum fluctuations.", 45));
	DialogueMap.emplace(45, DialogueLine(45, "Now...\nTry to find the\nProfessor", 46));
	DialogueMap.emplace(46, DialogueLine(46, "He'll know what\nto do next.", 0));

	//Doc1 here we go
	DialogueMap.emplace(47, DialogueLine(47, "Ok! Here we go!", 0));


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
