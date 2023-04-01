#include "EntityManager.h"
#include <algorithm>
#include <vector>

std::vector<Entity*> EnitityManager::EntityList;

void EnitityManager::Update(float dt)
{
	for (Entity* e : EntityList)
	{
		e->update(dt);
	}
}

void EnitityManager::Add(Entity* e)
{
	EntityList.push_back(e);
}

void EnitityManager::Remove(Entity* e)
{
	auto newEnd = std::remove(EntityList.begin(), EntityList.end(), e);
	EntityList.erase(newEnd, EntityList.end());
}
