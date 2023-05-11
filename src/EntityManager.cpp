#include "EntityManager.h"
#include <algorithm>
#include <vector>
#include <string>

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
	std::vector<Entity*>::iterator new_end;
	new_end = remove(EntityList.begin(), EntityList.end(), e);
	std::cout << "entity list size " + std::to_string(EntityList.size()) << std::endl;
	
}
