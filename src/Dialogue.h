#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>

enum DialogueState
{
	Writing, Idle
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

class DialogueManager
{
public:
	static std::unordered_map<int, Dialogue> DialogueMap;
	static void InitDialogues();
};

class DialogueBox 
{
public:
	DialogueBox(Vector2 pos, Vector2 widht_height, int start)
	{

	}



};