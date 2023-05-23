#pragma once
#include <string>
namespace settings
{
	
	const int tileSize = 32;
	const int ScreenScale = 2;
	const int drawSize = tileSize * ScreenScale;
	//constexpr int screenWidth = 800;
	//constexpr int screenHeight = 600;
	//constexpr int screenWidth = 1280;
	//constexpr int screenHeight = 720;
	//constexpr int screenWidth = 1366;
	//constexpr int screenHeight = 768;
	//constexpr int screenWidth = 1600;
	//constexpr int screenHeight = 900;
	constexpr int screenWidth = 1920;
	constexpr int screenHeight = 1080;
	constexpr int viewSizeWidth = 640;
	constexpr int viewSizeHeight = 320;
	const float zoom =  (float)screenWidth / (float)viewSizeWidth / (float) ScreenScale * 1.00f;
	constexpr int fps = 60;
	const std::string windowTitle = "Test";
	constexpr int PhysicsWorldScale = tileSize / ScreenScale;
	const int PhysicsTileUnit = tileSize / PhysicsWorldScale;

}