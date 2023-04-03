#include "Animations.h"
#include <unordered_map>
#include <raylib.h>
#include <iostream>

std::unordered_map<std::string, Texture2D> TextureLoader::m_Textures;

void TextureLoader::LoadTextures()
{
	m_Textures.emplace("BIG_Z", LoadTexture("res/bigzombie.png"));
	m_Textures.emplace("Z_SPAWNER", LoadTexture("res/zSpawner.png"));
}

void Animations::InitializeBigZAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("BIG_Z");
	animations.emplace("BIGZ_IDLE", *(new Animation(texture, 0, 10, 32, 0.1f)));
	animations.emplace("BIGZ_DOWN", *(new Animation(texture, 1, 8, 32, 0.1f)));
	animations.emplace("BIGZ_UP", *(new Animation(texture, 2, 8, 32, 0.1f)));
	animations.emplace("BIGZ_RIGHT", *(new Animation(texture, 3, 9, 32, 0.1f)));
	animations.emplace("BIGZ_LEFT", *(new Animation(texture, 4, 9, 32, 0.1f)));
}
void Animations::InitializeZSpawnerAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("Z_SPAWNER");

	Animation* idle_anim = new Animation(texture, 0, 10, 32, 0.1f); //Animation with custom frame times
	idle_anim->SetCustomFrameTime(3, 0.3f);							//Set custom frame time before adding to vector
	animations.emplace("ZSPAWNER_IDLE", *idle_anim);
	
	animations.emplace("ZSPAWNER_EMERGE", *(new Animation(texture, 1, 10, 32, 0.1f))); // standard animation
}


Animation* Animations::GetAnimation(std::string name)
{
	return &animations.at(name);
}


Texture2D* TextureLoader::GetTexture(std::string name)
{
	return &m_Textures.at(name);
}


Animation::Animation(Texture2D* texture, int spriteSheetRow, int framesCount, int frameSize, float frameTime)
	:
	m_texture(texture),
	m_spriteSheetRow(spriteSheetRow * frameSize),
	m_framesCount(framesCount),
	m_frameSize(frameSize),
	m_frameTime(frameTime)
{
	BuildFrameRectangles(framesCount);
	BuildFrameTimes(framesCount);
	m_currentFrame = m_frames[0];
	m_animationTicker = m_framesTimes[0];
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
			if (!m_playOnce) 
			{
				m_currentFrameNum = 0;
			}
			else
			{
				m_currentFrameNum = m_framesCount - 1;
				m_reachedEnd = true;
			}
		}
		m_currentFrame = m_frames[m_currentFrameNum];
		m_animationTicker = m_framesTimes[m_currentFrameNum];
	}
}

void Animation::PlayOnce()
{
	m_currentFrameNum = 0;
	m_reachedEnd = false;
	m_playOnce = true;
}

bool Animation::AnimationEnded()
{
	return m_reachedEnd;
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

const Rectangle Animation::GetCurrentFrame()
{
	return m_currentFrame;
}


