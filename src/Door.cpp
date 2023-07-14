#include "Door.h"
#include "LevelManager.h"
Door::Door(const Rectangle& rect, bool is_right)
	:
	Collidable(rect, b2_staticBody, DOOR),
	m_is_right(is_right)
{
	InitAnimations();
	//sensor collider box
	b2PolygonShape sensor_shape;
	sensor_shape.SetAsBox(2.8f, 1.0f, b2Vec2(0.0f, 0.0f), 0);
	//fixture user data
	FixtureUserData* sensorFixtureName = new FixtureUserData;
	sensorFixtureName->name = "door_sensor";
	//fixture definition
	b2FixtureDef sensorFixDef;
	sensorFixDef.isSensor = true;
	sensorFixDef.shape = &sensor_shape;
	sensorFixDef.userData.pointer = reinterpret_cast<uintptr_t>(sensorFixtureName);
	//create fixture using definition
	sensor = m_body->CreateFixture(&sensorFixDef);

	state = DoorState::Closed;
	EnitityManager::Add(this);


}

Door::~Door()
{
	EnitityManager::Remove(this);

}


void Door::Update(float dt)
{

	player_in_sensor = LevelManager::CheckPlayerInSensor(*sensor);
	// Swtich animation frames for current anim
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
			if (AnimationEnded())
			{
				state = DoorState::Closed;
			}
		}
		else
		{
			state = DoorState::Opening;
			//transfer current anim frame num to new anim
			int frameNum = 17 - animation->GetCurrentFrameNum();
			PlayFromFrame(frameNum, "D_OPEN");
		}
		break;
	case DoorState::Opening:
		if (!player_in_sensor)
		{
			state = DoorState::Closing;
			//transfer current anim frame num to new anim
			int frameNum = 17 - animation->GetCurrentFrameNum();
			PlayFromFrame(frameNum, "D_CLOSE");
		}
		else
		{
			if (AnimationEnded())
			{
				state = DoorState::Open;
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
