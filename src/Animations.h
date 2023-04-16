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
	bool AnimationEnded();
	void FreezeFrame(int frameNumber);


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
	bool m_playOnce = false;
	bool m_reachedEnd = false;
	bool m_active = false;
};

class Animations
{
public:
	void InitializePlayerAnimations();
	void InitializeBigZAnimations();
	void InitializeZSpawnerAnimations();
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
		animation = animations->GetAnimation(name);
		animations->m_CurrentActiveAnimation = name;

	}
	void FreezeFrame(std::string name, int frameNumber)
	{
		SetAnimation(name);
		animation->FreezeFrame(frameNumber);
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
	bool AnimationEnded()
	{
		return animation->AnimationEnded();
	}
};