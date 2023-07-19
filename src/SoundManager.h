#pragma once
#include <unordered_map>
#include <raylib.h>
#include <string>


class SoundManager 
{
public:
	static std::unordered_map<std::string, Sound> sounds;
	static void LoadSounds();
	static void PlayRandSounds(std::string*, int count);
};