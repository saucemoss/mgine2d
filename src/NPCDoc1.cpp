#include "NPCDoc1.h"
#include "Shaders.h"
#include "GameScreen.h"

NPCDoc1::NPCDoc1(const Rectangle& rect) :
	Collidable({ rect.x,rect.y, 16,22 }, b2_kinematicBody, NPC)
{
	//m_fixture->SetSensor(true);
	util::SimpleSensor(m_body, "npc_sensor", 1.0f, 1.0f, -1.0f, 0.0f);
	util::SimpleSensor(m_body, "npc_fear_sensor", 3.0f, 1.0f, 1.0f, 0.0f);

	InitAnimations();

	EnitityManager::Add(this);
}

NPCDoc1::~NPCDoc1()
{
	EnitityManager::Remove(this);
}

void NPCDoc1::Draw(int l)
{
	auto spritePosX = center_pos().x - 8;
	auto spritePosY = center_pos().y + 1;
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

void NPCDoc1::Update(float dt)
{
	SwitchFrames(dt);

	if (player_in_sensor)
	{
		auto spritePosX = center_pos().x - 8;
		auto spritePosY = center_pos().y + 1;
		Rectangle cframe = looking_right ? CurrentFrame() : Rectangle{ CurrentFrame().x,
																	CurrentFrame().y,
																	CurrentFrame().width * -1,
																	CurrentFrame().height };
		Rectangle source = { spritePosX,spritePosY,settings::tileSize,settings::tileSize };
		GameScreen::shaders->ApplyOutline(*sprite, cframe, source, { 0,0 }, 0.0f);
	}

}

void NPCDoc1::InitAnimations()
{
	sprite = TextureLoader::GetTexture("NPC_DOC1");
	animations->InitializeNPCDoc1Animations();
	SetAnimation("SEC1_IDLE");
}
