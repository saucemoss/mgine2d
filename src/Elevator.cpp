#include "Elevator.h"
#include "GameScreen.h"


Elevator::Elevator(const Rectangle& rect, ldtk::ArrayField<int> levels, const ldtk::Entity& ldtk_elevator)
	:
	Collidable(rect, b2_kinematicBody, ELEVATOR),
	m_ldtk_elevator(ldtk_elevator)
{
	InitAnimations();

	m_fixture->SetSensor(true);
	//walls:
	//b2Vec2 vs[4];
	//vs[0].Set(-1.0f, -1.0f);
	//vs[1].Set(-1.0f, 1.0f);
	//vs[2].Set(1.0f, 1.0f);
	//vs[3].Set(1.0f, -1.0f);
	//b2ChainShape chain;
	//chain.CreateLoop(vs, 4);
	////fixture user data
	//FixtureUserData* elevator_walls = new FixtureUserData;
	//elevator_walls->name = "elevator_walls";
	////fixture definition
	//b2FixtureDef walls_fix_def;
	//walls_fix_def.shape = &chain;
	//walls_fix_def.userData.pointer = reinterpret_cast<uintptr_t>(elevator_walls);
	////create fixture using definition
	//walls_fixture = m_body->CreateFixture(&walls_fix_def);

	//floor&roof
	b2PolygonShape floor_box;
	floor_box.SetAsBox(1, 0.1f, b2Vec2(0, 1.1f), 0);
	b2PolygonShape roof_box;
	roof_box.SetAsBox(1, 0.1f, b2Vec2(0, -1.1f), 0);
	//fixture user data
	FixtureUserData* floor_name = new FixtureUserData;
	FixtureUserData* roof_name = new FixtureUserData;
	floor_name->name = "floor_box";
	roof_name->name = "roof_box";
	//fixture definition
	b2FixtureDef floor_f_def;
	floor_f_def.shape = &floor_box;
	floor_f_def.userData.pointer = reinterpret_cast<uintptr_t>(floor_name);
	b2FixtureDef roof_f_def;
	roof_f_def.shape = &roof_box;
	roof_f_def.userData.pointer = reinterpret_cast<uintptr_t>(roof_name);
	//create fixture using definition
	m_body->CreateFixture(&floor_f_def);
	m_body->CreateFixture(&roof_f_def);

	m_levels = levels;
	for (int i = 0; i < m_levels.size(); i++)
	{
		m_levels.at(i) = m_levels.at(i).value() * settings::tileSize + settings::tileSize / 2;
	}

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
	
	player_in_sensor = LevelManager::CheckPlayerInSensor(*m_fixture);
	

	int lastLevel = (m_levels.back().value() - m_levels[0].value());
	
	switch (state)
	{
	case ElevatorState::START_LEVEL:
		if (player_in_sensor && IsKeyPressed(KEY_E) && 
			pos().y == m_levels[0].value())
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
		
		next_level = m_levels[m_current_level + 1].value();
		if (pos().y >= next_level)
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

		next_level = m_levels[m_current_level - 1].value();
		if (pos().y <= next_level)
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
		if (next_level > pos().y)
		{
			MoveDown(m_speed);
			m_going_up = false;
		}
		else
		{
			MoveUp(m_speed);
			m_going_up = true;
		}

		if (next_level <= pos().y + 5 &&
			next_level >= pos().y - 5 )
		{
			ResetY(next_level);
			for (int i = 0; i < m_levels.size(); i++)
			{
				if (m_levels.at(i).value() == next_level)
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
	
	//if (open)
	//{
	//	walls_fixture->SetSensor(true);
	//}
	//else
	//{
	//	walls_fixture->SetSensor(false);
	//}
}

void Elevator::ResetY(int next_level)
{
	
	m_body->SetTransform({ m_body->GetPosition().x, next_level / settings::PPM}, 0);
	m_body->SetLinearVelocity({ 0, 0 });
	//open = true;
}

void Elevator::MoveUp(float speed)
{
	m_body->SetLinearVelocity({ 0, -speed });
	//open = false;
}

void Elevator::MoveDown(float speed)
{
	m_body->SetLinearVelocity({ 0, speed });
	//open = false;

}

void Elevator::MoveToSwitch(float y_in)
{
	
	next_level = y_in;
	state = ElevatorState::GOING_TO_SW;
	//open = false;
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
