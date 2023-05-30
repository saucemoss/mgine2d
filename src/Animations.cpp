#include "Animations.h"
#include <unordered_map>
#include <raylib.h>
#include <iostream>


std::unordered_map<std::string, Texture2D> TextureLoader::m_Textures;

void TextureLoader::LoadTextures()
{
	m_Textures.emplace("BIG_Z", LoadTexture("res/bigzombie.png"));
	m_Textures.emplace("PLAYER", LoadTexture("res/PlayerTextures/player32.png"));
	m_Textures.emplace("NP1", LoadTexture("res/PlayerTextures/new_player2.png"));
	m_Textures.emplace("DOOR", LoadTexture("res/level/door.png"));
	m_Textures.emplace("DECOR_ANIM", LoadTexture("res/level/decor_anim.png"));

}


void Animations::InitializeDecorAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("DECOR_ANIM");
	animations.emplace("DECOR_169", *(new Animation(texture, 0, 5, 32, 0.12f)));
	animations.emplace("DECOR_170", *(new Animation(texture, 1, 5, 32, 0.12f)));
	animations.emplace("DECOR_244", *(new Animation(texture, 2, 6, 32, 0.25f)));
	animations.emplace("DECOR_162", *(new Animation(texture, 3, 2, 32, 0.50f)));
	//skip for elevator
	//animations.emplace("tbd", *(new Animation(texture, 5, tbd, tbd, tbd)));
	
}

void Animations::InitializeDoorAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("DOOR");
	animations.emplace("D_OPEN", *(new Animation(texture, 0, 17, 32, 0.02f)));
	animations.emplace("D_CLOSE", *(new Animation(texture, 1, 17, 32, 0.02f)));
	
}

void Animations::InitializePlayerAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("NP1");
	animations.emplace("P_RUN", *(new Animation(texture, 0, 6, 32, 0.08f)));
	animations.emplace("P_GROUND", *(new Animation(texture, 1, 1, 32, 0.08f)));
	animations.emplace("P_FALL", *(new Animation(texture, 2, 1, 32, 0.08f)));
	animations.emplace("P_IDLE", *(new Animation(texture, 3, 2, 32, 0.28f)));

	Animation* jump_anim = new Animation(texture, 4, 3, 32, 0.08f);
	jump_anim->SetCustomFrameTime(1, 0.10f);
	jump_anim->SetCustomFrameTime(2, 0.14f);
	jump_anim->SetCustomFrameTime(3, 0.5f);
	animations.emplace("P_JUMP", *jump_anim);

}

void Animations::InitializeElevatorAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("DECOR_ANIM");
	animations.emplace("ELEV_IDLE", *(new Animation(texture, 4, 1, 32, 0.12f)));
	animations.emplace("ELEV_SW_IDLE", *(new Animation(texture, 5, 3, 32, 0.12f)));
	animations.emplace("ELEV_SW_PRESSED", *(new Animation(texture, 6, 4, 32, 0.06f)));
	animations.emplace("ELEV_CLOSE", *(new Animation(texture, 7, 8, 32, 0.05f)));
	animations.emplace("ELEV_OPEN", *(new Animation(texture, 8, 8, 32, 0.05f)));
	
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
		m_currentFrame = m_frames[m_currentFrameNum];
		m_animationTicker = m_framesTimes[m_currentFrameNum];
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
			}
			m_reachedEnd = true;
		}
	}
}

void Animation::PlayOnce()
{
	m_playOnce = true;
}

void Animation::Reset()
{
	m_currentFrameNum = 0;
	m_animationTicker = m_framesTimes[m_currentFrameNum];
	m_currentFrame = m_frames[m_currentFrameNum];
	m_reachedEnd = false;
}

bool Animation::AnimationEnded()
{
	return m_reachedEnd;
}

void Animation::FreezeFrame(int frameNumber)
{
	m_currentFrameNum = frameNumber-1;
	m_currentFrame = m_frames[m_currentFrameNum];
	m_animationTicker = 1.0f;
}

void Animation::SetToFrame(int frameNumber)
{
	m_currentFrameNum = frameNumber-1;
	m_currentFrame = m_frames[m_currentFrameNum];
	m_animationTicker = m_framesTimes[m_currentFrameNum];
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
	m_framesTimes[frameNumber-1] = duration;
}

const Rectangle Animation::GetCurrentFrame()
{
	return m_currentFrame;
}

int Animation::GetCurrentFrameNum()
{
	return m_currentFrameNum;
}


