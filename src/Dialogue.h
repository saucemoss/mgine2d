#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>


enum DialogueState
{
	Writing, Idle, Exhausted
};

class Dialogue
{
public:
	Dialogue(int id, std::string text, int next_id)
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
	static std::unordered_map<int, Dialogue> DialogueMap;
	static void InitDialogues();
	static std::vector<std::unique_ptr<DialogueBox>> DialogueBoxes;
	static bool DialogBoxExhausted()
	{
		return state == Exhausted;
	}

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
		state = Exhausted;
		DialogueBoxes.clear();
	}
	static void StartDialogue(int i, Vector2 pos)
	{
		if (state != Writing)
		{
			state = Writing;
			DialogueBoxes.push_back(std::make_unique<DialogueBox>(Vector2{ pos.x - 32.0f, pos.y - 40.0f }, Vector2{ 90.0f, 30.0f }, i));
		}
		
	}
};






