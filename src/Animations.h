#pragma once
#include <string>
#include <unordered_map>
#include <raylib.h>
#include <vector>
#include <iostream>

class Animation
{
public:

	void SetCustomFrameTime(int frameNumber, float duration);
	Rectangle GetCurrentFrame();
	Animation(Texture2D& texture, int spriteSheetRow, int framesCount, int frameSize, float frameTime);
	void SwitchFrames(float dt);
	~Animation()
	{
		std::cout << "Deleted animation" << std::endl;
	}

private:
	const Texture2D* m_texture;
	const int m_spriteSheetRow;
	const int m_framesCount;
	const int m_frameSize;
	const float m_frameTime;
	float m_animationTicker;
	std::vector<Rectangle> m_frames;
	std::vector<float> m_framesTimes;
	void BuildFrameRectangles(int framesCount);
	void BuildFrameTimes(int framesCount);
	Rectangle m_currentFrame;
	int m_currentFrameNum;

};

class Animations
{
public:
	const Texture2D& GetTexture();
	Texture2D m_texture;
	void InitializeBigZAnimations();
	Animation* GetAnimation(std::string name);
private:
	std::unordered_map<std::string, Animation> animations;
	

};