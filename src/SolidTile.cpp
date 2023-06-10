#include "SolidTile.h"

SolidTile::SolidTile(const Rectangle& rectangle)
	: Collidable(rectangle, b2_staticBody)
{
	m_colliderTag = SOLID;
}

SolidTile::~SolidTile()
{
}


