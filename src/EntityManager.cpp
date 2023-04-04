#include "EntityManager.h"
#include <algorithm>
#include <vector>

std::vector<Entity*> EnitityManager::EntityList;

void EnitityManager::Update(float dt)
{
	for (Entity* e : EntityList)
	{
		e->Update(dt);
	}
}

void EnitityManager::Draw()
{
	for (Entity* e : EntityList)
	{
		e->Draw();
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
