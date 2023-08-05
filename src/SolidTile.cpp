#include "SolidTile.h"

SolidTile::SolidTile(const Rectangle& rectangle)
	: Collidable(rectangle, b2_staticBody, SOLID)
{
}

SolidTile::SolidTile(const Rectangle& rectangle, bool is_floor)
	: 
	Collidable(rectangle, b2_staticBody, SOLID),
	is_floor(is_floor)
{}

SolidTile::SolidTile(const b2Vec2* verices, const int vertices_count, const Rectangle& rectangle)
	: Collidable(verices, vertices_count, b2_staticBody, SOLID, rectangle)
{
	is_slope = true;
}

SolidTile::~SolidTile()
{
}


