#include "NPCSecurityGuy2.h"

NPCSecurityGuy2::NPCSecurityGuy2(const Rectangle& rect) :
	Collidable({ rect.x,rect.y, 16,22 }, b2_kinematicBody, NPC)
{
	//m_fixture->SetSensor(true);
	util::SimpleSensor(m_body, "npc_sensor", 1.0f, 1.0f, -1.0f, 0.0f);
	util::SimpleSensor(m_body, "npc_fear_sensor", 2.0f, 1.0f, 1.0f, 0.0f);
	state = Idling;
	InitAnimations();

	EnitityManager::Add(this);
}

NPCSecurityGuy2::~NPCSecurityGuy2()
{
	EnitityManager::Remove(this);
}

void NPCSecurityGuy2::Draw(int l)
{
	auto spritePosX = center_pos().x - 8;
	auto spritePosY = center_pos().y;
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

void NPCSecurityGuy2::Update(float dt)
{
	SwitchFrames(dt);

	switch (state)
	{
	case Idling:
		if (enemy_in_sensor)
		{
			SetAnimation("SEC2_RUN");
			state = Running;
		}
		break;
	case Running:
		looking_right = false;
		m_body->SetLinearVelocity({
		-5.0f,
		m_body->GetLinearVelocity().y,
			});
		break;
	}

}

void NPCSecurityGuy2::InitAnimations()
{
	sprite = TextureLoader::GetTexture("NPC_S_GUY2");
	animations->InitializeNPCSec2Animations();
	SetAnimation("SEC2_GUN");
}
