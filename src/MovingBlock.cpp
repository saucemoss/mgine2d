#include "MovingBlock.h"
#include "Settings.h"
#include "GameScreen.h"
#include "raymath.h"


MovingBlock::MovingBlock(const Rectangle& rect, const ldtk::ArrayField<ldtk::IntPoint> path_points)
	:m_path_points(path_points)
{
	InitAnimations();
	rectangle = rect;
	x = rectangle.x;
	y = rectangle.y;
	w = rectangle.width;
	h = rectangle.height; 
	m_colliderTag = M_BLOCK;
	m_top_part = { x+10,y,w-20,20 };
	AddColliderElement(&m_top_part, M_BLOCK_TOP);
	EnitityManager::Add(this);
	m_next_point = m_path_points.at(0).value();

}

MovingBlock::~MovingBlock()
{
	EnitityManager::Remove(this);
}

void MovingBlock::Draw()
{
	auto spritePosX = x;
	auto spritePosY = y;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,settings::drawSize,settings::drawSize },
		{ 0,0 },
		0.0f,
		WHITE);
}

void MovingBlock::Update(float dt)
{
	speed = 35;
	m_next_point = m_path_points.at(m_path_step_counter).value();
	int px = m_next_point.x * settings::drawSize;
	int py = m_next_point.y * settings::drawSize;

	if (px > x)
	{
		vx += speed;
	}
	if (px < x)
	{
		vx -= speed;
	}
	if (py > y)
	{
		vy += speed;
	}
	if (py < y)
	{
		vy -= speed;
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
	//Apply friction
	float friction = 1800;
	if (vx > 0.0f)
	{
		vx = std::max(0.0f, vx - friction * dt);
	}
	else if (vx < 0.0f)
	{
		vx = std::min(0.0f, vx + friction * dt);
	}
	if (vy > 0.0f)
	{
		vy = std::max(0.0f, vy - friction * dt);
	}
	else if (vy < 0.0f)
	{
		vy = std::min(0.0f, vy + friction * dt);
	}


	x += vx * dt;
	y += vy * dt;
	rectangle.x = x;
	rectangle.y = y;
	m_top_part.x = x + 10;
	m_top_part.y = y;
	for (auto& e : elements)
	{
		e->rectangle.x = m_top_part.x;
		e->rectangle.y = m_top_part.y;
	}
}

void MovingBlock::DrawCollider()
{
	DrawRectangleLinesEx(rectangle, 1, RED);
}

void MovingBlock::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeDecorAnimations();
	FreezeFrame("M_BLOCK",1);
}
