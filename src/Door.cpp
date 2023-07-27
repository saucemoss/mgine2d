#include "Door.h"
#include "LevelManager.h"
#include "SoundManager.h"
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
	if (player_in_sensor)
	{
		m_fixture->SetSensor(true);
	}
	else
	{
		m_fixture->SetSensor(false);
	}

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
			}
		}
		else
		{
			state = DoorState::Opening;
			//transfer current anim frame num to new anim
			int frameNum = 17 - animation->GetCurrentFrameNum();
			PlayFromFrame(frameNum, "D_OPEN");
			animation->PlayOnce();
		}
		break;
	case DoorState::Opening:
		if (!player_in_sensor)
		{
			state = DoorState::Closing;
			//transfer current anim frame num to new anim
			int frameNum = 17 - animation->GetCurrentFrameNum();
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
			}
		}
		break;
	}


}

void Door::Draw(int l)
{
	auto spritePosX = center_pos().x;
	auto spritePosY = center_pos().y;

	Rectangle cframe = m_is_right ? CurrentFrame() : Rectangle{		CurrentFrame().x,
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

void Door::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DOOR");
	animations->InitializeDoorAnimations();
	FreezeFrame("D_CLOSE",17);
}
