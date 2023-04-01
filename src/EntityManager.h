#pragma once
#include <vector>
#include "Entity.h"

class EnitityManager
{
public:
	static std::vector<Entity*> EntityList;
	static void Update(float dt);
	static void Add(Entity*);
	static void Remove(Entity*);
};