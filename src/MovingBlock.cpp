#include "MovingBlock.h"
#include "Settings.h"
#include "GameScreen.h"
#include "raymath.h"


MovingBlock::MovingBlock(const Rectangle& rect, const ldtk::ArrayField<ldtk::IntPoint> path_points)
	:
	Collidable(rect, b2_kinematicBody),
	m_path_points(path_points)
{
	InitAnimations();

	m_colliderTag = M_BLOCK;
	EnitityManager::Add(this);
	m_next_point = m_path_points.at(0).value();

}

MovingBlock::~MovingBlock()
{
	EnitityManager::Remove(this);
}

void MovingBlock::Draw()
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

void MovingBlock::Update(float dt)
{
	float speed = 2;
	m_next_point = m_path_points.at(m_path_step_counter).value();
	int x = center_pos().x;
	int y = center_pos().y;
	int px = m_next_point.x * settings::tileSize;
	int py = m_next_point.y * settings::tileSize;

	if (px > x)
	{
		m_body->SetLinearVelocity({ speed, 0 });

	}
	if (px < x)
	{
		m_body->SetLinearVelocity({ -speed, 0 });
	}
	if (py > y)
	{
		m_body->SetLinearVelocity({ 0, speed });
	}
	if (py < y)
	{
		m_body->SetLinearVelocity({ 0, -speed });
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

	m_rectangle =
	{
		pos().x - m_rectangle.width / 2,
		pos().y - m_rectangle.height / 2,
		m_rectangle.width,
		m_rectangle.height
	};

}

void MovingBlock::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeDecorAnimations();
	FreezeFrame("M_BLOCK",1);
}
