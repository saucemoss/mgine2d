#pragma once
#include <string>
#include <raylib.h>


static std::string VecToString(Vector2 vec)
{
	return std::to_string((int)vec.x) + ", " + std::to_string((int)vec.y);
}
