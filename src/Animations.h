#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <raylib.h>
#include <vector>
#include <iostream>

class Animation 
{
public:
	Animation(Texture2D* texture, int spriteSheetRow, int framesCount, int frameSize, float frameTime);
	~Animation()
	{
		//std::cout << "Deleted animation" << std::endl;
 	}

	void SetCustomFrameTime(int frameNumber, float duration);
	const Rectangle GetCurrentFrame();
	void SwitchFrames(float dt);
	void PlayOnce();
	void Reset();
	bool AnimationEnded();
	void FreezeFrame(int frameNumber);
	void SetToFrame(int frameNumber);
	int GetCurrentFrameNum();
	const Texture2D* GetTexture()
	{
		return m_texture;
	}


private:
	const Texture2D* m_texture;
	const int m_spriteSheetRow;
	const int m_framesCount;
	const int m_frameSize;
	const float m_frameTime;
	int m_currentFrameNum;
	float m_animationTicker;
	std::vector<Rectangle> m_frames;
	std::vector<float> m_framesTimes;
	void BuildFrameRectangles(int framesCount);
	void BuildFrameTimes(int framesCount);
	Rectangle m_currentFrame;
	
	bool m_playOnce = false;
	bool m_reachedEnd = false;
};

class Animations
{
public:
	void InitializePlayerAnimations();
	void InitializeElevatorAnimations();
	void InitializeRolloAnimations();
	void InitializeIvanAnimations();
	void InitializeEntanglerAnimations();
	void InitializeNPCDoc1Animations();
	void InitializeMediPodAnimations();
	void InitializeBoss1Animations();
	void InitializeSwitchAnimations();
	void InitializeBossGlassAnimations();
	void InitializeGateAnimations();
	void InitializeAxePickupAnimations();
	void InitializeTerminalAnimations();
	void InitializeWCArmAnimations();
	void InitializeNPCSec1Animations();
	void InitializeNPCSec2Animations();
	void InitializeRibbsAnimations();
	void InitializeFootballAnimations();
	void InitializeLeggyAnimations();
	void InitializeFlyingInfectedAnimations();
	void InitializeInfectedHAnimations();
	void InitializeDecorAnimations();
	void InitializeDoorAnimations();
	void InitializeHeadSpitAnimations();
	Animation* GetAnimation(std::string name);
	std::string m_CurrentActiveAnimation;
private:
	std::map<std::string, Animation> animations;
	
};

class TextureLoader
{
public:
	static std::unordered_map<std::string, Texture2D> m_Textures;
	static void LoadTextures();
	static Texture2D* GetTexture(std::string name);
};

class Animated
{
private:
	bool m_uninterupt = false;
public:
	Animation* animation = nullptr;
	Animations* animations = new Animations();
	Texture2D* sprite = nullptr;
	~Animated()
	{
		delete animations;
	}
	virtual void InitAnimations() = 0;
	void SetAnimation(std::string name)
	{
		if(m_uninterupt)
		{ 
			if (AnimationEnded())
			{
				animation = animations->GetAnimation(name);
				animations->m_CurrentActiveAnimation = name;
				animation->Reset();
				m_uninterupt = false;
			}
			else
			{
				//wait
			}
		}
		else
		{
			animation = animations->GetAnimation(name);
			animation->Reset();
			animations->m_CurrentActiveAnimation = name;
		}
	}
	void PlayAnimation(std::string name)
	{
		if (m_uninterupt)
		{
			if (AnimationEnded())
			{
				animation = animations->GetAnimation(name);
				animations->m_CurrentActiveAnimation = name;
				m_uninterupt = false;
			}
			else
			{
				//wait
			}
		}
		else
		{
			animation = animations->GetAnimation(name);
			animations->m_CurrentActiveAnimation = name;
		}
	}
	void FreezeFrame(std::string name, int frameNumber)
	{
		animation = animations->GetAnimation(name);
		animation->FreezeFrame(frameNumber-1);
		animations->m_CurrentActiveAnimation = name;
		
	}
	void PlayFromFrame(int frameNum, std::string name)
	{
		SetAnimation(name);
		animation->SetToFrame(frameNum);
	}
	void SwitchFrames(float dt) const
	{
		animation->SwitchFrames(dt);
	}
	Rectangle CurrentFrame() const
	{
		return animation->GetCurrentFrame();
	}
	void PlayOnce(std::string name)
	{
		SetAnimation(name);
		animation->PlayOnce();
	}
	void PlayOnceUninterupt(std::string name)
	{
		PlayOnce(name);
		m_uninterupt = true;
	}
	bool AnimationEnded()
	{
		return animation->AnimationEnded();
	}
	bool AnimationEnded(std::string anim_name)
	{
		return animation->AnimationEnded() && animations->m_CurrentActiveAnimation == anim_name;
	}
	bool IsCurrentAnimation(std::string name)
	{
		return name == animations->m_CurrentActiveAnimation;
	}
};