#include "NPCSecurityGuy.h"

NPCSecurityGuy::NPCSecurityGuy(const Rectangle& rect):
Collidable({rect.x,rect.y, 16,22 }, b2_kinematicBody, NPC)
{
	//m_fixture->SetSensor(true);
	util::SimpleSensor(m_body, "npc_sensor", 1.0f, 1.0f, -1.0f, 0.0f);
	util::SimpleSensor(m_body, "npc_fear_sensor", 3.0f, 1.0f, 1.0f, 0.0f);
	state = Idling;
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

	switch (state)
	{
	case Idling:
		if (enemy_in_sensor)
		{
			SetAnimation("SEC1_RUN");
			state = Running;
		}
		if (player_in_sensor)
		{
			if (DialogueManager::DialogBoxExhausted())
			{
				state = TalkingOption2;
				SetAnimation("SEC1_TALK");
			}
			else
			{
				state = TalkingOption1;
				SetAnimation("SEC1_STOP");
			}
		}
		break;
	case TalkingOption1:
		if (enemy_in_sensor)
		{
			SetAnimation("SEC1_RUN");
			state = Running;
		}
		if (!player_in_sensor)
		{
			state = Idling;
			SetAnimation("SEC1_IDLE");
			DialogueManager::EndDialogue();
		}
		else if (!DialogueManager::DialogBoxExhausted())
		{
			DialogueManager::StartDialogue(1, center_pos());
		}

		break;
	case TalkingOption2:
		if (enemy_in_sensor)
		{
			SetAnimation("SEC1_RUN");
			state = Running;
		}
		if (!player_in_sensor)
		{
			state = Idling;
			SetAnimation("SEC1_IDLE");
			DialogueManager::EndDialogue();
		}
		else
		{
			DialogueManager::StartDialogue(6, center_pos());
		}
		break;

	case Running:
		looking_right = false;
		m_body->SetLinearVelocity({
		-4.0f,
		m_body->GetLinearVelocity().y,
			});
		break;
	}
	
}

void NPCSecurityGuy::InitAnimations()
{
	sprite = TextureLoader::GetTexture("NPC_S_GUY1");
	animations->InitializeNPCSec1Animations();
	SetAnimation("SEC1_IDLE");
}
