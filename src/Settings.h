#pragma once
#include <string>
namespace settings
{
	//Window settings
	const int ScreenScale = 2;
	constexpr int screenWidth = 1600;
	constexpr int screenHeight = 900;
	constexpr int fps = 60;
	const std::string windowTitle = "Test";
	constexpr int PhysicsWorldScale = 32.0f / ScreenScale;

}