#include "NPCSecurityGuy.h"

NPCSecurityGuy::NPCSecurityGuy(const Rectangle& rect):
Collidable({rect.x,rect.y, 16,22 }, b2_kinematicBody, NPC)
{
	//m_fixture->SetSensor(true);
	util::SimpleSensor(m_body, "npc_sensor", 2.0f, 1.0f, 0.0f, 0.0f);
	InitAnimations();
	EnitityManager::Add(this);
}

NPCSecurityGuy::~NPCSecurityGuy()
{
	EnitityManager::Remove(this);
}

void NPCSecurityGuy::Draw(int l)
{
	auto spritePosX = center_pos().x - 8;
	auto spritePosY = center_pos().y ;
	Rectangle cframe = looking_right ? CurrentFrame() : Rectangle{ CurrentFrame().x,
																CurrentFrame().y,
																CurrentFrame().width * -1,
																CurrentFrame().height };
	DrawTexturePro(*sprite,
			cframe,
			Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
			{ 0,0 },
			0.0f,
			WHITE);
	
}

void NPCSecurityGuy::Update(float dt)
{
	SwitchFrames(dt);
	if (AnimationEnded())
	{
		if (player_in_sensor)
		{
			SetAnimation("SEC1_TALK");
		}
		else
		{
			SetAnimation("SEC1_IDLE");
		}
	}

	
}

void NPCSecurityGuy::InitAnimations()
{
	sprite = TextureLoader::GetTexture("NPC_S_GUY1");
	animations->InitializeNPCSec1Animations();
	SetAnimation("SEC1_IDLE");
}
