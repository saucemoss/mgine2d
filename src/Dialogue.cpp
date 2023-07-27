#include "Dialogue.h"
std::unordered_map<int, Dialogue> DialogueManager::DialogueMap;
void DialogueManager::InitDialogues()
{
	DialogueMap.emplace(1, Dialogue(1, "STOP!", 2));
	DialogueMap.emplace(2, Dialogue(2, "Haven't you heard?\nThere was a security incident...", 3));
	DialogueMap.emplace(3, Dialogue(3, "But we have it under controll.", 0));
}
