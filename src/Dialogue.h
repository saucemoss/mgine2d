#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "GameScreen.h"


enum DialogueState
{
	Writing, Idle, Exhausted
};

class DialogueLine
{
public:
	DialogueLine(int id, std::string text, int next_id)
		:
		id(id), text(text), next_id(next_id)
	{

	}
	int id;
	int next_id;
	std::string text;

};

class DialogueBox
{
public:
	DialogueBox(Vector2 pos, Vector2 widht_height, int start)
		:pos(pos), widht_height(widht_height), current_id(start)
	{
	}
	Vector2 pos;
	Vector2 widht_height;
	int current_id;
	void DrawDialogue();
	void UpdateDialogue(float dt);
	DialogueState state = Idle;
	float f_counter = 0.0f;

};

class DialogueManager
{
public:
	static DialogueState state;
	static std::unordered_map<int, DialogueLine> DialogueMap;
	static std::vector<int> used_lines;
	static void InitDialogues();
	static std::vector<std::unique_ptr<DialogueBox>> DialogueBoxes;

	static void DrawDialogues()
	{
		for (auto& d : DialogueManager::DialogueBoxes)
		{
			d->DrawDialogue();
		}
	}
	static void UpdateDialogues(float dt)
	{
		for (auto& d : DialogueManager::DialogueBoxes)
		{
			d->UpdateDialogue(dt);
		}
	}

	static void EndDialogue()
	{
		DialogueBoxes.clear();
		GameScreen::player->state = PlayerState::Idle;
		state = Exhausted;
	}
	static void StartDialogue(int i, Vector2 pos)
	{
		if (state != Writing)
		{
			GameScreen::player->state = PlayerState::InDialogue;
			state = Writing;
			DialogueBoxes.push_back(std::make_unique<DialogueBox>(Vector2{ pos.x - 32.0f, pos.y - 40.0f }, Vector2{ 90.0f, 30.0f }, i));
		}
		
	}

	static bool DialogueExhausted(int d)
	{
		return std::find(used_lines.begin(), used_lines.end(), d) != used_lines.end();
	}
	
	static void ResetDialogue(int d)
	{
		std::vector<int>::iterator it = std::find(used_lines.begin(), used_lines.end(), d);
		if (it != used_lines.end())
			used_lines.erase(it);
	}
};






