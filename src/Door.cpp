#include "Door.h"

Door::Door(const Rectangle& rect, bool is_right)
	:
	m_is_right(is_right)
{
	InitAnimations();
	rectangle = rect;
	x = rectangle.x;
	y = rectangle.y;
	w = rectangle.width;
	h = rectangle.height;

	rectangle.x += 15;
	rectangle.width -= 30;
	sensor = { x - 115,y,w + 230,h };
	m_colliderTag = DOOR;
	state = DoorState::Closed;
	EnitityManager::Add(this);
	CollisionManager::Add(this);

}

Door::~Door()
{
	EnitityManager::Remove(this);
	CollisionManager::Remove(this);

}


void Door::Update(float dt)
{
	
	player_in_sensor = CollisionManager::IsCollisionWith(PLAYER, sensor);

	switch (state)
	{
	case DoorState::Open:
		if (!player_in_sensor)
		{
			state = DoorState::Closing;
			SetAnimation("D_CLOSE");
		}
		else
		{
			FreezeFrame("D_OPEN", 17);	
		}
		break;
	case DoorState::Closed:
		if (!player_in_sensor)
		{
			FreezeFrame("D_CLOSE", 17);
		}
		else
		{
			state = DoorState::Opening;
			SetAnimation("D_OPEN");
		}
		break;
	case DoorState::Closing:
		if (!player_in_sensor)
		{
			CollisionManager::Add(this);
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
			CollisionManager::Remove(this);
			if (AnimationEnded())
			{
				state = DoorState::Open;
			}
		}
		break;
	}
	// Swtich animation frames for current anim
	SwitchFrames(dt);

}

void Door::Draw()
{
	auto spritePosX = x ;
	auto spritePosY = y ;

	Rectangle cframe = m_is_right ? CurrentFrame() : Rectangle{		CurrentFrame().x,
																	CurrentFrame().y,
																	CurrentFrame().width * -1,
																	CurrentFrame().height };
	DrawTexturePro(*sprite,
		cframe,
		Rectangle{ spritePosX,spritePosY,settings::drawSize,settings::drawSize },
		{ 0,0 },
		0.0f,
		WHITE);
}

void Door::DrawCollider()
{

	DrawRectangleLinesEx(rectangle, 1, BLUE);
	DrawRectangleLinesEx(sensor, 1, YELLOW);

	std::map<DoorState, std::string> StatesStrMap{ 
		{DoorState::Open,"Open"}, 
		{DoorState::Closed,"Closed"},
		{DoorState::Closing,"Closing"},
		{DoorState::Opening,"Opening"}
	};
	std::string stateStr = "State: " + StatesStrMap[state];
	DrawText(stateStr.c_str(), x, y - 50, 20, BLACK);
	std::string animStr = "Anim: " + animations->m_CurrentActiveAnimation + " :: " +
		std::to_string(animations->GetAnimation(animations->m_CurrentActiveAnimation)->GetCurrentFrameNum());
	DrawText(animStr.c_str(), x, y - 70, 20, BLACK);
	
}

void Door::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DOOR");
	animations->InitializeDoorAnimations();
	FreezeFrame("D_CLOSE",17);
}
