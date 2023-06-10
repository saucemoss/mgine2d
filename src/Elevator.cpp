#include "Elevator.h"
#include "GameScreen.h"

Elevator::Elevator(const Rectangle& rect, ldtk::ArrayField<int> levels, const ldtk::Entity& ldtk_elevator)
	:
	Collidable(rect, b2_kinematicBody),
	m_ldtk_elevator(ldtk_elevator)
{
	InitAnimations();

	m_levels = levels;
	m_colliderTag = ELEVATOR;
	state = ElevatorState::START_LEVEL;
	EnitityManager::Add(this);

}

Elevator::~Elevator()
{
	EnitityManager::Remove(this);
}

void Elevator::Update(float dt)
{
	SwitchFrames(dt);
	
	//player_in_sensor = CollisionManager::IsCollisionWith(PLAYER, sensor);
	int lastLevel = (m_levels.back().value() - m_levels[0].value()) * settings::tileSize;
	int level_position = m_body->GetPosition().y * settings::PPM * settings::tileSize;
	
	switch (state)
	{
	case ElevatorState::START_LEVEL:
		if (player_in_sensor && IsKeyPressed(KEY_E) && 
			m_body->GetPosition().y * settings::PPM == m_levels[0].value() * settings::tileSize)
		{
			state = ElevatorState::GOING_DOWN;
			m_current_level = 0;
		}
		else if (player_in_sensor && IsKeyPressed(KEY_E))
		{
			state = ElevatorState::GOING_UP;
			m_current_level = m_levels.size();
		}
		break;
	case ElevatorState::GOING_DOWN:
		MoveDown(m_speed);
		
		next_level = m_levels[m_current_level + 1].value() * settings::tileSize;
		if (m_body->GetPosition().y * settings::PPM >= next_level)
		{
			ResetY(next_level);
			m_current_level++;
			state = ElevatorState::NEXT_LEVEL;
		}

		if (player_in_sensor && IsKeyPressed(KEY_E) && m_current_level != 0) //change of dir
		{
			state = ElevatorState::GOING_UP;
		}

		break;
	case ElevatorState::GOING_UP:
		MoveUp(m_speed);

		next_level = m_levels[m_current_level - 1].value() * settings::tileSize;
		if (m_body->GetPosition().y * settings::PPM <= next_level)
		{
			ResetY(next_level);
			m_current_level--;
			state = ElevatorState::PREVIOUS_LEVEL;
		}

		if (player_in_sensor && IsKeyPressed(KEY_E) && m_current_level != m_levels.size() - 1) //change of dir
		{
			state = ElevatorState::GOING_DOWN;
		}
		break;
	case ElevatorState::NEXT_LEVEL:
		if (player_in_sensor && IsKeyPressed(KEY_E) && m_current_level == m_levels.size()-1)
		{
			state = ElevatorState::GOING_UP; // last level
		}
		else if (player_in_sensor && IsKeyPressed(KEY_E))
		{
			state = ElevatorState::GOING_DOWN;
		} 
		break;
	case ElevatorState::PREVIOUS_LEVEL:
		if (player_in_sensor && IsKeyPressed(KEY_E) && m_current_level == 0)
		{
			state = ElevatorState::GOING_DOWN; // first level
		}
		else if (player_in_sensor && IsKeyPressed(KEY_E))
		{
			state = ElevatorState::GOING_UP;
		}
		break;
	case ElevatorState::GOING_TO_SW:
		if (next_level > m_body->GetPosition().y * settings::PPM)
		{
			MoveDown(m_speed);
			m_going_up = false;
		}
		else
		{
			MoveUp(m_speed);
			m_going_up = true;
		}

		if (next_level <= m_body->GetPosition().y * settings::PPM + 5 && next_level >= m_body->GetPosition().y * settings::PPM - 5 )
		{
			ResetY(next_level);
			for (int i = 0; i < m_levels.size(); i++)
			{
				if (m_levels.at(i).value() * settings::tileSize == next_level)
				{
					m_current_level = i;
					break;
				}
			}
			state = ElevatorState::AT_SW;
		}
		break;
	case ElevatorState::AT_SW:
		if (player_in_sensor && IsKeyPressed(KEY_E) && m_current_level == 0)
		{
			state = ElevatorState::GOING_DOWN; // first level
		}
		else if (player_in_sensor && IsKeyPressed(KEY_E) && m_current_level == m_levels.size() - 1)
		{
			state = ElevatorState::GOING_UP;
		}
		else if (player_in_sensor && IsKeyPressed(KEY_E) && !m_going_up)
		{
			state = ElevatorState::GOING_DOWN;
		}
		else if (player_in_sensor && IsKeyPressed(KEY_E) && m_going_up)
		{
			state = ElevatorState::GOING_UP;
		}
		break;
	}
	//std::cout << m_current_level << std::endl;
}

void Elevator::ResetY(int next_level)
{
	
	m_body->SetTransform({ next_level + m_rectangle.height, m_body->GetPosition().x * settings::PPM }, 0);
}

void Elevator::MoveUp(float speed)
{
	m_body->SetLinearVelocity({ -speed, 0 });
}

void Elevator::MoveDown(float speed)
{
	m_body->SetLinearVelocity({ speed, 0 });

}

void Elevator::MoveToSwitch(float y_in)
{
	
	next_level = y_in;
	state = ElevatorState::GOING_TO_SW;
}

void Elevator::Draw()
{
	auto spritePosX = center_pos().x;
	auto spritePosY = center_pos().y;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
		{ 0,0 },
		0.0f,
		WHITE);
}

//void Elevator::DrawCollider()
//{
//	DrawRectangleLinesEx(m_rectangle, 1, RED);
//
//	std::map<ElevatorState, std::string> StatesStrMap{
//		{ElevatorState::START_LEVEL,"START_LEVEL"},
//		{ElevatorState::GOING_DOWN,"GOING_DOWN"},
//		{ElevatorState::GOING_UP,"GOING_UP"},
//		{ElevatorState::GOING_TO_SW,"GOING_TO_SW"},
//		{ElevatorState::NEXT_LEVEL,"NEXT_LEVEL"},
//		{ElevatorState::PREVIOUS_LEVEL, "PREVIOUS_LEVEL"},
//		{ElevatorState::AT_SW, "AT_SW"},
//	};
//	std::string stateStr = "State: " + StatesStrMap[state];
//	DrawText(stateStr.c_str(), m_rectangle.x, m_rectangle.y - 50, 20, BLACK);
//}

void Elevator::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeElevatorAnimations();
	SetAnimation("ELEV_IDLE");
	
}
