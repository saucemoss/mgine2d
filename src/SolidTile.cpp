#include "SolidTile.h"
#include "Settings.h"
#include "Collisions.h"

SolidTile::SolidTile()
{
}

SolidTile::SolidTile(const Rectangle& rect)
{
	rectangle = rect;
	x = rectangle.x;
	y = rectangle.y;
	w = rectangle.width;
	h = rectangle.height;
	m_colliderTag = SOLID;
	CollisionManager::Add(this);
}


SolidTile::~SolidTile()
{
	CollisionManager::Remove(this);
}

void SolidTile::DrawCollider()
{
	DrawRectangleLinesEx(rectangle, 1, RED);
	if (colliding) 
	{
		DrawRectangleRec(rectangle,RED);
	}
}

