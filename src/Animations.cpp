#include "Animations.h"
#include <raylib.h>
#include <iostream>


void Animations::InitializeBigZAnimations() // build and load animations into memory
{
	m_texture = LoadTexture("res/bigzombie.png");
	animations.emplace("BIG_ZOMBIE_IDLE", *(new Animation(m_texture, 0, 10, 32, 0.1f)));
	animations.emplace("BIG_ZOMBIE_DOWN", *(new Animation(m_texture, 1, 8, 32, 0.1f)));
	animations.emplace("BIG_ZOMBIE_UP", *(new Animation(m_texture, 2, 8, 32, 0.1f)));
	animations.emplace("BIG_ZOMBIE_RIGHT", *(new Animation(m_texture, 3, 9, 32, 0.1f)));
	animations.emplace("BIG_ZOMBIE_LEFT", *(new Animation(m_texture, 4, 9, 32, 0.1f)));
}

Animation* Animations::GetAnimation(std::string name)
{
	return &animations.at(name);
}

const Texture2D& Animations::GetTexture()
{
	return m_texture;
}

Animation::Animation(Texture2D& texture, int spriteSheetRow, int framesCount, int frameSize, float frameTime)
	:
	m_texture(&texture),
	m_spriteSheetRow(spriteSheetRow * frameSize),
	m_framesCount(framesCount),
	m_frameSize(frameSize),
	m_frameTime(frameTime)
{
	std::cout << "Creating Animation" << std::endl;
	BuildFrameRectangles(framesCount);
	BuildFrameTimes(framesCount);
	m_currentFrame = m_frames[0];
	m_animationTicker = frameTime;
}

void Animation::SwitchFrames(float dt)
{
	m_animationTicker -= dt;
	if (m_animationTicker <= 0)
	{
		if (m_currentFrameNum < m_framesCount-1)
		{
			m_currentFrameNum +=1 ;
		}
		else
		{
			m_currentFrameNum = 0;
		}
		m_currentFrame = m_frames[m_currentFrameNum];
		m_animationTicker = m_framesTimes[m_currentFrameNum];
	}
}

void Animation::BuildFrameRectangles(int framesCount)
{

	m_frames.resize(framesCount);
	for (int i = 0; i < framesCount; i++)
	{
		m_frames[i] = Rectangle{ (float)i * m_frameSize,(float)m_spriteSheetRow,(float)m_frameSize,(float)m_frameSize };
	}
}

void Animation::BuildFrameTimes(int framesCount)
{
	m_framesTimes.resize(framesCount);
	for (int i = 0; i < framesCount; i++)
	{
		m_framesTimes[i] = m_frameTime;
	}
}

void Animation::SetCustomFrameTime(int frameNumber, float duration)
{
	m_framesTimes[frameNumber] = duration;
}

Rectangle Animation::GetCurrentFrame()
{
	return m_currentFrame;
}


