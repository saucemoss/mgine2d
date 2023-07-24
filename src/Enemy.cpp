#include "Enemy.h"
#include "GameScreen.h"


Enemy::Enemy(const Rectangle& rectangle, ColliderTag tag) :
	Collidable({ rectangle.x,rectangle.y,rectangle.width,rectangle.height}, b2_dynamicBody, tag)
{
	EnitityManager::Add(this);
	state = EnemyState::Idle;

	// Add mappings for debug purposes
	StatesStrMap[EnemyState::Idle] = "Idle";
	StatesStrMap[EnemyState::Flying] = "Flying";
	StatesStrMap[EnemyState::Attacking] = "Attacking";
	StatesStrMap[EnemyState::Dying] = "Dying";
	StatesStrMap[EnemyState::Running] = "Running";
	StatesStrMap[EnemyState::Hurting] = "Hurting";

}

void Enemy::CheckTouchGround()
{
	is_touching_floor = false;
	if (ground_contacts > 0)
	{
		is_touching_floor = true;
	}
}

void Enemy::set_velocity_x(float vx)
{
	m_body->SetLinearVelocity({
	vx,
	m_body->GetLinearVelocity().y,});
}

void Enemy::set_velocity_y(float vy)
{
	m_body->SetLinearVelocity({
	m_body->GetLinearVelocity().x,
	vy,
	});
}

void Enemy::set_velocity_xy(float vx, float vy)
{
	m_body->SetLinearVelocity({ vx, vy });
}

void Enemy::Draw(int l)
{
	Rectangle cframe = looking_right ? CurrentFrame() : Rectangle{ CurrentFrame().x,
																CurrentFrame().y,
																CurrentFrame().width * -1,
																CurrentFrame().height };
	
	if (!custom_pos)
	{
		spritePosX = center_pos().x - 10;
		spritePosY = center_pos().y - 1;
	}

	if (CurrentFrame().width == 96 && !custom_pos)
	{
		spritePosX = center_pos().x - 42;
		spritePosY = center_pos().y - 33;
	}

	Color c = WHITE;
	if (taking_dmg)
	{
		c = int(dmg_counter*9) % 2 == 0 ? RED : WHITE;
	}
	if (state == EnemyState::Hurting)
	{
		c = RED;
	}

	DrawTexturePro(*animation->GetTexture(),
		cframe,
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ 0,0 },
		0.0f,
		c);

	if (GameScreen::debug)
	{
		DrawText(std::to_string(m_current_hp).c_str(), center_pos().x, center_pos().y - 10, 10, RED);
		std::string stateStr = "" + StatesStrMap[state];
		DrawText(stateStr.c_str(), center_pos().x, center_pos().y -50, 10, BLACK);
	}
}
