#include "LevelPortal.h"


LevelPortal::LevelPortal()
{
}

LevelPortal::LevelPortal(const Rectangle& rect, std::string to_level, float xNewPlayerPos, float yNewPlayerPos)
	:
	m_to_level(to_level),
	m_xNewPlayerPos(xNewPlayerPos),
	m_yNewPlayerPos(yNewPlayerPos)
{
	rectangle = rect;
	x = rectangle.x;
	y = rectangle.y;
	w = rectangle.width;
	h = rectangle.height;
	m_colliderTag = LEVEL_PORTAL;
	CollisionManager::Add(this);
	EnitityManager::Add(this);

}


LevelPortal::~LevelPortal()
{
	CollisionManager::Remove(this);
	EnitityManager::Remove(this);
}

void LevelPortal::Draw()
{
}

void LevelPortal::DrawCollider()
{
	DrawRectangleLinesEx(rectangle, 1, BLUE);
}

void LevelPortal::Update(float dt)
{
}
