#include "MovingBlock.h"
#include "Settings.h"
#include "GameScreen.h"
#include "raymath.h"


MovingBlock::MovingBlock(const Rectangle& rect, const ldtk::ArrayField<ldtk::IntPoint> path_points)
	:
	Collidable(rect, b2_kinematicBody, M_BLOCK),
	m_path_points(path_points)
{
	InitAnimations();

	EnitityManager::Add(this);
	m_next_point = m_path_points.at(0).value();

}

MovingBlock::~MovingBlock()
{
	EnitityManager::Remove(this);
}

void MovingBlock::Draw(int l)
{
	auto spritePosX = center_pos().x;
	auto spritePosY = center_pos().y - 4.0f;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
		{ 0,0 },
		0.0f,
		WHITE);
}

void MovingBlock::Update(float dt)
{
	float speed = 2;
	m_next_point = m_path_points.at(m_path_step_counter).value();
	int x = center_pos().x;
	int y = center_pos().y;
	int px = m_next_point.x * settings::tileSize;
	int py = m_next_point.y * settings::tileSize + 4.0f;
	m_body->SetLinearVelocity({ 0, 0 });
	if (px > x)
	{
		m_body->SetLinearVelocity({ speed, m_body->GetLinearVelocity().y});
	}
	if (px < x)
	{
		m_body->SetLinearVelocity({ -speed, m_body->GetLinearVelocity().y });
	}
	if (py > y)
	{
		m_body->SetLinearVelocity({ m_body->GetLinearVelocity().x, speed });
	}
	if (py < y)
	{
		m_body->SetLinearVelocity({ m_body->GetLinearVelocity().x, -speed });
	}


	if ((int)py == (int)y && (int)px == (int)x)
	{
		if (m_path_step_counter < m_path_points.size()-1)
		{
			m_path_step_counter++;
		}
		else
		{
			m_path_step_counter = 0;
		}
	}


}

void MovingBlock::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeDecorAnimations();
	FreezeFrame("M_BLOCK",1);
}
