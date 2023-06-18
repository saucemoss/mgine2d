#include "SolidTile.h"

SolidTile::SolidTile(const Rectangle& rectangle)
	: Collidable(rectangle, b2_staticBody, SOLID)
{
}

SolidTile::~SolidTile()
{
}


