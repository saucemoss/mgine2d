#include "Animations.h"
#include <unordered_map>
#include <raylib.h>
#include <iostream>


std::unordered_map<std::string, Texture2D> TextureLoader::m_Textures;

void TextureLoader::LoadTextures()
{
	m_Textures.emplace("PLAYER", LoadTexture("res/PlayerTextures/player32.png"));
	m_Textures.emplace("NP1", LoadTexture("res/PlayerTextures/new_player2.png"));
	m_Textures.emplace("DOOR", LoadTexture("res/level/door.png"));
	m_Textures.emplace("DECOR_ANIM", LoadTexture("res/level/decor_anim.png"));
	m_Textures.emplace("MOTHMAN", LoadTexture("res/level/mothman.png"));
	m_Textures.emplace("P_ATT_1", LoadTexture("res/PlayerTextures/p_attack_1.png"));
	m_Textures.emplace("INFECTED_H_1", LoadTexture("res/enemies/infected_hazmat.png"));
	m_Textures.emplace("FLY_INF_1", LoadTexture("res/enemies/flying_infected.png"));
	m_Textures.emplace("RIBBS_1", LoadTexture("res/enemies/ribbs.png"));
	m_Textures.emplace("LEGGY_1", LoadTexture("res/enemies/legs.png"));
	m_Textures.emplace("FOOTB_1", LoadTexture("res/enemies/football.png"));
	m_Textures.emplace("HSPIT_1", LoadTexture("res/enemies/headspit.png"));
	m_Textures.emplace("NPC_S_GUY1", LoadTexture("res/NPCs/NPCSecurityGuy.png"));
	m_Textures.emplace("NPC_S_GUY2", LoadTexture("res/NPCs/NPCSecutiry2.png"));
}

void Animations::InitializeNPCSec1Animations()
{
	Texture2D* texture = TextureLoader::GetTexture("NPC_S_GUY1");
	animations.emplace("SEC1_IDLE", *(new Animation(texture, 0, 4, 32, 0.25f)));
	animations.emplace("SEC1_LETGO", *(new Animation(texture, 1, 1, 32, 0.12f)));
	animations.emplace("SEC1_STOP", *(new Animation(texture, 2, 4, 32, 0.12f)));
	animations.emplace("SEC1_TALK", *(new Animation(texture, 3, 4, 32, 0.18f)));
	animations.emplace("SEC1_RUN", *(new Animation(texture, 4, 2, 32, 0.04f)));
}
void Animations::InitializeNPCSec2Animations()
{
	Texture2D* texture = TextureLoader::GetTexture("NPC_S_GUY2");
	animations.emplace("SEC2_IDLE", *(new Animation(texture, 0, 1, 32, 0.25f)));
	animations.emplace("SEC2_RUN", *(new Animation(texture, 1, 2, 32, 0.04f)));
	animations.emplace("SEC2_GUN", *(new Animation(texture, 2, 1, 32, 0.25f)));
}

void Animations::InitializeRibbsAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("RIBBS_1");
	animations.emplace("RIBBS_ATT", *(new Animation(texture, 0, 7, 96, 0.08f)));
	animations.emplace("RIBBS_RUN", *(new Animation(texture, 4, 4, 32, 0.08f)));
	animations.emplace("RIBBS_IDLE", *(new Animation(texture, 5, 5, 32, 0.28f)));
	animations.emplace("RIBBS_DMG", *(new Animation(texture, 6, 1, 32, 0.26f)));

	Animation* RIBBS_DEAD = new Animation(texture, 3, 9, 32, 0.06f);
	RIBBS_DEAD->SetCustomFrameTime(9, 3.12f);
	animations.emplace("RIBBS_DEAD", *RIBBS_DEAD);
}

void Animations::InitializeFootballAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("FOOTB_1");

	Animation* FOOTB_ATT = new Animation(texture, 0, 12, 32, 0.04f);
	FOOTB_ATT->SetCustomFrameTime(7, 0.03f);
	animations.emplace("FOOTB_ATT", *FOOTB_ATT);

	animations.emplace("FOOTB_IDLE", *(new Animation(texture, 1, 5, 32, 0.28f)));
	animations.emplace("FOOTB_DMG", *(new Animation(texture, 1, 1, 32, 0.18f)));

	Animation* FOOTB_DEAD = new Animation(texture, 2, 6, 32, 0.06f);
	FOOTB_DEAD->SetCustomFrameTime(6, 3.12f);
	animations.emplace("FOOTB_DEAD", *FOOTB_DEAD);
}

void Animations::InitializeLeggyAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("LEGGY_1");
	animations.emplace("LEGGY_STAND", *(new Animation(texture, 0, 9, 96, 0.10f)));
	animations.emplace("LEGGY_IDLE", *(new Animation(texture, 1, 5, 96, 0.08f)));
	animations.emplace("LEGGY_RUN", *(new Animation(texture, 2, 5, 96, 0.08f)));
	animations.emplace("LEGGY_ATT", *(new Animation(texture, 5, 5, 96, 0.08f)));
	animations.emplace("LEGGY_DMG", *(new Animation(texture, 3, 1, 96, 0.32f)));

	Animation* LEGGY_DEAD = new Animation(texture, 4, 11, 96, 0.08f);
	LEGGY_DEAD->SetCustomFrameTime(11, 2.00f);
	animations.emplace("LEGGY_DEAD", *LEGGY_DEAD);
}

void Animations::InitializeFlyingInfectedAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("FLY_INF_1");
	animations.emplace("FLY_I_IDLE", *(new Animation(texture, 0, 5, 96, 0.06f)));
	animations.emplace("FLY_I_FLY", *(new Animation(texture, 0, 5, 96, 0.06f)));
	animations.emplace("FLY_I_ATT", *(new Animation(texture, 1, 6, 96, 0.06f)));
	animations.emplace("FLY_I_DMG", *(new Animation(texture, 3, 1, 96, 0.18f)));
	animations.emplace("FLY_I_DEAD", *(new Animation(texture, 2, 6, 96, 0.06f)));
}

void Animations::InitializeInfectedHAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("INFECTED_H_1");
	animations.emplace("IH_IDLE", *(new Animation(texture, 0, 3, 32, 0.16f)));
	animations.emplace("IH_RUN", *(new Animation(texture, 1, 3, 32, 0.08f)));
	animations.emplace("IH_ATT", *(new Animation(texture, 2, 10, 32, 0.08f)));
	animations.emplace("IH_DMG", *(new Animation(texture, 3, 1, 32, 0.16f)));

	Animation* IH_DEATH = new Animation(texture, 4, 10, 32, 0.08f);
	IH_DEATH->SetCustomFrameTime(3, 0.05f);
	IH_DEATH->SetCustomFrameTime(4, 0.05f);
	IH_DEATH->SetCustomFrameTime(5, 0.05f);
	IH_DEATH->SetCustomFrameTime(6, 0.05f);
	IH_DEATH->SetCustomFrameTime(7, 0.08f);
	IH_DEATH->SetCustomFrameTime(8, 0.10f);
	IH_DEATH->SetCustomFrameTime(9, 0.12f);
	IH_DEATH->SetCustomFrameTime(10, 3.12f);
	animations.emplace("IH_DEATH", *IH_DEATH);

	Animation* IH_DEATH2 = new Animation(texture, 2, 16, 96, 0.04f);
	IH_DEATH2->SetCustomFrameTime(7, 0.05f);
	IH_DEATH2->SetCustomFrameTime(8, 0.05f);
	IH_DEATH2->SetCustomFrameTime(9, 0.05f);
	IH_DEATH2->SetCustomFrameTime(10, 0.06f);
	IH_DEATH2->SetCustomFrameTime(11, 0.08f);
	IH_DEATH2->SetCustomFrameTime(12, 0.10f);
	IH_DEATH2->SetCustomFrameTime(13, 0.12f);
	IH_DEATH2->SetCustomFrameTime(14, 0.22f);
	IH_DEATH2->SetCustomFrameTime(15, 0.52f);
	IH_DEATH2->SetCustomFrameTime(16, 1.12f);
	animations.emplace("IH_DEATH2", *IH_DEATH2);

	Animation* IH_DEATH3 = new Animation(texture, 3, 16, 96, 0.04f);
	IH_DEATH3->SetCustomFrameTime(7, 0.05f);
	IH_DEATH3->SetCustomFrameTime(8, 0.05f);
	IH_DEATH3->SetCustomFrameTime(9, 0.05f);
	IH_DEATH3->SetCustomFrameTime(10, 0.06f);
	IH_DEATH3->SetCustomFrameTime(11, 0.08f);
	IH_DEATH3->SetCustomFrameTime(12, 0.08f);
	IH_DEATH3->SetCustomFrameTime(13, 0.08f);
	IH_DEATH3->SetCustomFrameTime(14, 0.08f);
	animations.emplace("IH_DEATH3", *IH_DEATH3);

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
	animations.emplace("DECOR_212", *(new Animation(texture, 9, 6, 32, 0.08f)));
	animations.emplace("M_BLOCK", *(new Animation(texture, 11, 1, 32, 0.50f)));
	
}

void Animations::InitializeDoorAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("DOOR");
	animations.emplace("D_OPEN", *(new Animation(texture, 0, 17, 32, 0.02f)));
	animations.emplace("D_CLOSE", *(new Animation(texture, 1, 17, 32, 0.02f)));
	
}

void Animations::InitializeHeadSpitAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("HSPIT_1");

	animations.emplace("HSPIT_EXP", *(new Animation(texture, 0, 7, 96, 0.06f)));
	animations.emplace("HSPIT_PROJ", *(new Animation(texture, 3, 5, 32, 0.12f)));
	animations.emplace("HSPIT_IDLE", *(new Animation(texture, 4, 4, 32, 0.12f)));
	animations.emplace("HSPIT_DEAD", *(new Animation(texture, 5, 5, 32, 0.08f)));
	animations.emplace("HSPIT_ATT", *(new Animation(texture, 6, 9, 32, 0.06f)));
	animations.emplace("HSPIT_DMG", *(new Animation(texture, 3, 1, 32, 0.18f)));
}

void Animations::InitializePlayerAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("NP1");
	animations.emplace("P_RUN1", *(new Animation(texture, 0, 6, 32, 0.08f)));
	animations.emplace("P_GROUND1", *(new Animation(texture, 1, 1, 32, 0.08f)));
	animations.emplace("P_FALL1", *(new Animation(texture, 2, 1, 32, 0.08f)));
	animations.emplace("P_IDLE1", *(new Animation(texture, 3, 2, 32, 0.28f)));
	Animation* jump_anim1 = new Animation(texture, 4, 3, 32, 0.08f);
	jump_anim1->SetCustomFrameTime(1, 0.10f);
	jump_anim1->SetCustomFrameTime(2, 0.14f);
	jump_anim1->SetCustomFrameTime(3, 0.5f);
	animations.emplace("P_JUMP1", *jump_anim1);
	animations.emplace("P_MELT", *(new Animation(texture, 10, 16, 32, 0.06f)));
	animations.emplace("P_SHOOT", *(new Animation(texture, 11, 1, 32, 0.16f)));
	animations.emplace("P_AXE_PICK", *(new Animation(texture, 13, 1, 32, 0.16f)));
	
	Animation* P_AXE_THROW1 = new Animation(texture, 14, 16, 32, 0.09f);
	P_AXE_THROW1->SetCustomFrameTime(1, 0.04f);
	P_AXE_THROW1->SetCustomFrameTime(2, 0.04f);
	P_AXE_THROW1->SetCustomFrameTime(3, 0.06f);
	P_AXE_THROW1->SetCustomFrameTime(4, 0.06f);
	P_AXE_THROW1->SetCustomFrameTime(5, 0.08f);
	P_AXE_THROW1->SetCustomFrameTime(6, 0.10f);
	P_AXE_THROW1->SetCustomFrameTime(7, 0.12f);
	P_AXE_THROW1->SetCustomFrameTime(8, 0.16f);
	P_AXE_THROW1->SetCustomFrameTime(9, 0.18f);
	P_AXE_THROW1->SetCustomFrameTime(10, 0.20f);
	P_AXE_THROW1->SetCustomFrameTime(11, 0.26f);
	P_AXE_THROW1->SetCustomFrameTime(12, 0.09f);
	P_AXE_THROW1->SetCustomFrameTime(13, 0.08f);
	P_AXE_THROW1->SetCustomFrameTime(14, 0.09f);
	P_AXE_THROW1->SetCustomFrameTime(15, 0.08f);
	P_AXE_THROW1->SetCustomFrameTime(16, 0.09f);
	animations.emplace("P_AXE_THROW1", *P_AXE_THROW1);

	animations.emplace("P_AXE_THROW2", *(new Animation(texture, 15, 2, 32, 0.06f)));

	animations.emplace("P_RUN", *(new Animation(texture, 5, 6, 32, 0.08f)));
	animations.emplace("P_GROUND", *(new Animation(texture, 7, 1, 32, 0.08f)));
	animations.emplace("P_FALL", *(new Animation(texture, 8, 1, 32, 0.08f)));
	animations.emplace("P_HURT", *(new Animation(texture, 16, 1, 32, 0.15f)));
	animations.emplace("P_IDLE", *(new Animation(texture, 6, 5, 32, 0.20f)));
	Animation* jump_anim = new Animation(texture, 9, 3, 32, 0.08f);
	jump_anim->SetCustomFrameTime(1, 0.10f);
	jump_anim->SetCustomFrameTime(2, 0.14f);
	jump_anim->SetCustomFrameTime(3, 0.5f);
	animations.emplace("P_JUMP", *jump_anim);


	Animation* P_ATT1 = new Animation(texture, 6, 5, 96, 0.02f);
	P_ATT1->SetCustomFrameTime(1, 0.05f);
	P_ATT1->SetCustomFrameTime(2, 0.05f);
	P_ATT1->SetCustomFrameTime(3, 0.07f);
	P_ATT1->SetCustomFrameTime(4, 0.08f);
	P_ATT1->SetCustomFrameTime(5, 0.09f);
	animations.emplace("P_ATT1", *P_ATT1);

	Animation* P_ATT2 = new Animation(texture, 7, 5, 96, 0.02f);
	P_ATT2->SetCustomFrameTime(1, 0.05f);
	P_ATT2->SetCustomFrameTime(2, 0.05f);
	P_ATT2->SetCustomFrameTime(3, 0.07f);
	P_ATT2->SetCustomFrameTime(4, 0.08f);
	P_ATT2->SetCustomFrameTime(5, 0.09f);
	animations.emplace("P_ATT2", *P_ATT2);





}

void Animations::InitializeElevatorAnimations()
{
	Texture2D* texture = TextureLoader::GetTexture("DECOR_ANIM");
	animations.emplace("ELEV_IDLE", *(new Animation(texture, 12, 1, 32, 0.12f)));
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
		m_currentFrameNum += 1;
		if (m_currentFrameNum >= m_framesCount)
		{
			if (!m_playOnce)
			{
				Reset();
			}
			else
			{
				m_currentFrameNum = m_framesCount-1;
			}
			m_reachedEnd = true;
		}
		m_currentFrame = m_frames[m_currentFrameNum];
		m_animationTicker = m_framesTimes[m_currentFrameNum];
	}
}

void Animation::PlayOnce()
{
	m_playOnce = true;
}

void Animation::Reset()
{
	m_currentFrameNum = 0;
	m_animationTicker = m_framesTimes[0];
	m_currentFrame = m_frames[0];
	m_reachedEnd = false;
}

bool Animation::AnimationEnded()
{
	return m_reachedEnd;
}

void Animation::FreezeFrame(int frameNumber)
{
	m_currentFrameNum = frameNumber;
	m_currentFrame = m_frames[m_currentFrameNum];
	m_animationTicker = 100000.0f;
	m_reachedEnd = false;
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


