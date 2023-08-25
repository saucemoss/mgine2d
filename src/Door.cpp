#include "Door.h"
#include "LevelManager.h"
#include "SoundManager.h"
#include "GameScreen.h"
#include "Terminal.h"
Door::Door(const Rectangle& rect, bool is_right)
	:
	Collidable(rect, b2_staticBody, DOOR),
	m_is_right(is_right)
{
	InitAnimations();
	//sensor collider box
	sensor = util::SimpleSensor(m_body, "door_sensor", 2.8f, 1.0f, 0.0f, 0.0f);
	my_engine_sound = LoadSound("res/sound/collected/Engine.wav");
	state = DoorState::Closed;

	EnitityManager::Add(this);

}

Door::~Door()
{
	EnitityManager::Remove(this);
	UnloadSound(my_engine_sound);
}


void Door::Update(float dt)
{
	SwitchFrames(dt);

	if (locked)
	{
		state = DoorState::Locked;
	}

	if (player_in_sensor && !locked)
	{
		m_fixture->SetSensor(true);
	}
	else
	{
		m_fixture->SetSensor(false);
	}

	if (player_in_sensor && locked && !IsSoundPlaying(SoundManager::sounds["denied"]) && !denied_played)
	{
		PlaySound(SoundManager::sounds["denied"]);
		denied_played = true;
	}
	if (!player_in_sensor) denied_played = false;


	switch (state)
	{
	case DoorState::Open:
		StopSound(my_engine_sound);
		if (!player_in_sensor)
		{
			state = DoorState::Closing;
			PlayOnce("D_CLOSE");	
		}
		else
		{
	
		}
		break;
	case DoorState::Closed:
		StopSound(my_engine_sound);
		if (!player_in_sensor)
		{

		}
		else
		{
			state = DoorState::Opening;
			PlayOnce("D_OPEN");
		}
		break;
	case DoorState::Closing:
		if (!player_in_sensor)
		{
			if(!IsSoundPlaying(my_engine_sound)) PlaySound(my_engine_sound);
			
			if (AnimationEnded())
			{
				state = DoorState::Closed;
				PlaySound(SoundManager::sounds["door_close"]);
				GameScreen::add_trauma(0.5f);
				ParticleEmitter* p = new ParticleEmitter(pos());
				GameScreen::Particles->Add(DefinedEmitter::dust, p);
				p->EmitParticles();
			}
		}
		else
		{
			state = DoorState::Opening;
			//transfer current anim frame num to new anim
			int frameNum = 7 - animation->GetCurrentFrameNum();
			PlayFromFrame(frameNum, "D_OPEN");
			animation->PlayOnce();
		}
		break;
	case DoorState::Opening:
		if (!player_in_sensor)
		{
			state = DoorState::Closing;
			//transfer current anim frame num to new anim
			int frameNum = 7 - animation->GetCurrentFrameNum();
			PlayFromFrame(frameNum, "D_CLOSE");
			animation->PlayOnce();
		}
		else
		{
			if (!IsSoundPlaying(my_engine_sound)) PlaySound(my_engine_sound);
			if (AnimationEnded())
			{
				state = DoorState::Open;
				PlaySound(SoundManager::sounds["door_close"]);
				GameScreen::add_trauma(0.5f);

			}
		}
		break;
	case DoorState::Locked:
		terminal_unlocks = refs.size();
		SetAnimation("D_LOCKED");

		if (refs.size() != 0)
		{
			for (auto r : refs)
			{
				auto id = r.value().entity_iid;

				for (auto& e : GameScreen::LevelMgr->level_entities_safe)
				{
					if (e.get()->m_ldtkID == id)
					{
						Terminal* t = static_cast<Terminal*>(e.get());
						if (t->state == TerminalState::Pass)
						{
							terminal_unlocks--;
						}
					}
				}

			}

			if (terminal_unlocks == 0)
			{
				if (!IsSoundPlaying(SoundManager::sounds["pc_work"]))
				{
					locked = false;
					state = DoorState::Closed;
					PlaySound(SoundManager::sounds["robo5"]);
					FreezeFrame("D_CLOSE", 7);
					GameScreen::add_trauma(1.0f);
					ParticleEmitter* p = new ParticleEmitter({ pos().x, pos().y - 20 });
					GameScreen::Particles->Add(DefinedEmitter::dust, p);
					p->EmitParticles();
					ParticleEmitter* p2 = new ParticleEmitter({ pos().x, pos().y + 20 });
					GameScreen::Particles->Add(DefinedEmitter::dust, p2);
					p2->EmitParticles();
				}
			}
		}

		break;
	}


}

void Door::Draw(int l)
{
	auto spritePosX = center_pos().x -14.0f;
	auto spritePosY = center_pos().y;

	Rectangle cframe = m_is_right ? CurrentFrame() : Rectangle{		CurrentFrame().x,
																	CurrentFrame().y,
																	CurrentFrame().width * -1,
																	CurrentFrame().height };
	DrawTexturePro(*sprite,
		cframe,
		Rectangle{ spritePosX,spritePosY,40,40 },
		{ 0,0 },
		0.0f,
		WHITE);
	
}

void Door::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DOOR");
	animations->InitializeDoorAnimations();
	FreezeFrame("D_CLOSE",7);
}
