#include "NPCDoc1.h"
#include "Shaders.h"
#include "GameScreen.h"
#include "Entangeler.h"

NPCDoc1::NPCDoc1(const Rectangle& rect) :
	Collidable({ rect.x,rect.y, 16,22 }, b2_kinematicBody, NPC_DOC1)
{
	m_fixture->SetSensor(true);
	if (GameScreen::LevelMgr->currentLdtkLevel->name == "Level_3")
	{
		util::SimpleSensor(m_body, "npc_sensor", 2.0f, 2.0f, -6.0f, 7.0f);
		state = NPCDoc1State::Idle_first_meet;
		if (DialogueManager::DialogueExhausted(20))
		{
			visible = false;
			m_destroy = true;
		}

	}
	else if (GameScreen::LevelMgr->currentLdtkLevel->name == "Level_4")
	{
		util::SimpleSensor(m_body, "npc_sensor", 4.0f, 2.0f, 0.0f, 0.0f);
		state = NPCDoc1State::Idle;
	}


	InitAnimations();

	EnitityManager::Add(this);
}

NPCDoc1::~NPCDoc1()
{
	EnitityManager::Remove(this);
}

void NPCDoc1::CheckLookingSide()
{
	if (player_in_sensor && GameScreen::player->center_pos().x > center_pos().x)
	{
		looking_right = true;
	}
	else if (player_in_sensor && GameScreen::player->center_pos().x < center_pos().x)
	{
		looking_right = false;
	}
}

void NPCDoc1::Draw(int l)
{
	if (visible)
	{
		auto spritePosX = center_pos().x;
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

}

void NPCDoc1::Update(float dt)
{
	SwitchFrames(dt);
	ApplyOutlineOnSensor();


	switch (state)
	{
	case NPCDoc1State::Idle_first_meet:
		if (player_in_sensor)
		{
			CheckLookingSide();
			state = NPCDoc1State::Talk_first_meet;
			SetAnimation("DOC1_TALK");
		}

		if (GameScreen::LevelMgr->currentLdtkLevel->name == "Level_3")
		{
			if (DialogueManager::DialogueExhausted(20))
			{
				visible = false;
				m_destroy = true;
			}
		}
		break;
	case NPCDoc1State::Talk_first_meet:
		if (!DialogueManager::DialogueExhausted(20))
		{
			CheckLookingSide();
			DialogueManager::StartDialogue(11, { center_pos().x, center_pos().y + 80 });
		}
		else
		{
			DialogueManager::EndDialogue();
			visible = false;
			m_destroy = true;
		}
		break;

	case NPCDoc1State::Idle:
		if (player_in_sensor && (IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)))
		{
			CheckLookingSide();
			state = NPCDoc1State::Talk;
			SetAnimation("DOC1_TALK");
		}
		break;

	case NPCDoc1State::Talk:
		if (!DialogueManager::DialogueExhausted(24))
		{
			state = NPCDoc1State::Talk_welcome;
		}

		if (IsKeyPressed(KEY_E) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))
		{

			if (!DialogueManager::DialogueExhausted(27))
			{
				state = NPCDoc1State::Talk_idea;			
			} 
			else if (!DialogueManager::DialogueExhausted(28) && !terminals_on)
			{
				state = NPCDoc1State::Talk_help;
			}
			else if (!DialogueManager::DialogueExhausted(37))
			{
				state = NPCDoc1State::Talk_explain;
			}
			else if (!DialogueManager::DialogueExhausted(46))
			{
				state = NPCDoc1State::Talk_lore;
			}
			else if (player_in_sensor && GameScreen::player->m_throwing_unlocked)
			{
				DialogueManager::ResetDialogue(37);
				state = NPCDoc1State::Talk_explain;
			}
		}
		break;
	case NPCDoc1State::Talk_welcome:
		CheckLookingSide();
		if (!DialogueManager::DialogueExhausted(24) && IsGamepadButtonUp(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))
		{
			DialogueManager::StartDialogue(21, center_pos()); // welcome
		}
		else if (DialogueManager::DialogueExhausted(24))
		{
			state = NPCDoc1State::Talk;
			SetAnimation("DOC1_IDLE");
		}
		break;
	case NPCDoc1State::Talk_idea:
		CheckLookingSide();
		if (!DialogueManager::DialogueExhausted(27) && IsGamepadButtonUp(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))
		{
			DialogueManager::StartDialogue(25, center_pos()); // idea
		}
		else if(DialogueManager::DialogueExhausted(27))
		{
			state = NPCDoc1State::GoToTerminal;
			SetAnimation("DOC1_RUN");
		}
		break;
	case NPCDoc1State::Talk_help:
		for (Entity* e : EnitityManager::EntityList)
		{
			if (e->m_ldtkID.str() == "990f1190-3b70-11ee-8449-45fc34f2f040")
			{
				Entangeler* en = static_cast<Entangeler*>(e);
				if (en->state == EntangelerState::Loaded)
				{
					entangeler_loaded = true;
					terminals_on = en->terminals_on;
				}
			}
		}

		if (!DialogueManager::DialogueExhausted(28) && IsGamepadButtonUp(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) && entangeler_loaded && player_in_sensor)
		{
			DialogueManager::StartDialogue(28, center_pos()); // help with terminals
		}
		else if (DialogueManager::DialogueExhausted(28))
		{
			state = NPCDoc1State::Talk;
			SetAnimation("DOC1_WORK2");
			DialogueManager::ResetDialogue(28);
		}

		if (terminals_on && entangeler_loaded)
		{
			state = NPCDoc1State::GoToStartButton;
			SetAnimation("DOC1_RUN");
		}

		break;
	case NPCDoc1State::Talk_explain:

		if (!DialogueManager::DialogueExhausted(37) && IsGamepadButtonUp(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) && GameScreen::player->m_throwing_unlocked)
		{
			DialogueManager::StartDialogue(29, center_pos()); // explain controlls
		}
		else if (DialogueManager::DialogueExhausted(37))
		{
			state = NPCDoc1State::Talk;
			SetAnimation("DOC1_IDLE");
		}
		break;
	case NPCDoc1State::Talk_lore:
		if (!DialogueManager::DialogueExhausted(46) && IsGamepadButtonUp(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))
		{
			DialogueManager::StartDialogue(38, center_pos()); // lore, next steps
		}
		else if (DialogueManager::DialogueExhausted(46))
		{
			state = NPCDoc1State::Talk;
			SetAnimation("DOC1_IDLE");
		}
		break;
	case NPCDoc1State::GoToTerminal:
		m_body->SetLinearVelocity(b2Vec2{ speed,0.0f });
		looking_right = true;
		if (pos().x > 1796)
		{
			looking_right = false;
			state = NPCDoc1State::Talk;
			m_body->SetLinearVelocity(b2Vec2{ 0.0f,0.0f });
			SetAnimation("DOC1_WORK2");
		}

		for (Entity* e : EnitityManager::EntityList)
		{
			if (e->m_ldtkID.str() == "990f1190-3b70-11ee-8449-45fc34f2f040")
			{
				Entangeler* en = static_cast<Entangeler*>(e);
				if (en->terminals_on)
				{
					state = NPCDoc1State::GoToStartButton;
					SetAnimation("DOC1_RUN");

				}
			}
		}

		break;
	case NPCDoc1State::GoToStartButton:
		m_body->SetLinearVelocity(b2Vec2{ -speed,0.0f });
		looking_right = false;
		if (pos().x <= 1732)
		{

			if(!DialogueManager::DialogueExhausted(47)) DialogueManager::StartDialogue(47, center_pos()); // here we go
			
			m_body->SetLinearVelocity(b2Vec2{ 0.0f,0.0f });
			if (DialogueManager::DialogueExhausted(47))
			{
				state = NPCDoc1State::Talk;
				SetAnimation("DOC1_IDLE");
			}
		}
		break;

	}

}

void NPCDoc1::ApplyOutlineOnSensor()
{
	if (player_in_sensor)
	{
		auto spritePosX = center_pos().x;
		auto spritePosY = center_pos().y;
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
	SetAnimation("DOC1_IDLE");
}
