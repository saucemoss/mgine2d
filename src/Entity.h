#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "LDtkLoader/Project.hpp"

class Entity
{
public:
    virtual ~Entity() = default;
	int m_draw_layer = 0;
    virtual void Draw() = 0;
    virtual void Update(float dt) = 0;
	ldtk::IID m_ldtkID;;
	bool m_destroy = false;
};


class EnitityManager
{
public:
	static std::vector<Entity*> EntityList;

	static void Update(float dt)
	{
		for (Entity* e : EntityList)
		{
			e->Update(dt);
		}
		for (int i = 0; i < EntityList.size(); i++)
		{
			if (EntityList[i]->m_destroy)
			{
				Remove(EntityList[i]);
			}
		}
	}
	static void Draw(int draw_layer)
	{
		for (Entity* e : EntityList)
		{
			if (e->m_draw_layer == draw_layer && !e->m_destroy)
			{
				e->Draw();
			}
		}

	}
	static void Add(Entity* e)
	{
		EntityList.push_back(e);
	}
	static void Remove(Entity* e)
	{
		std::vector<Entity*>::iterator it = std::find(EntityList.begin(), EntityList.end(), e);
		if (it != EntityList.end())
			EntityList.erase(it);
	}
};
