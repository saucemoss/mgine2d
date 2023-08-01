#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "LDtkLoader/Project.hpp"
#include "Collidable.h"

class Entity
{
public:
    virtual ~Entity() = default;
	int m_layer = 0;
	int m_draw_layers;
    virtual void Draw(int layer) = 0;
	virtual void Update(float dt) = 0;
	ldtk::IID m_ldtkID;;
	bool m_destroy = false;
	bool queue_entity_add = false;
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
				Collidable* c = dynamic_cast<Collidable*>(EntityList[i]);
				if (c != nullptr)
				{
					c->m_body->GetWorld()->DestroyBody(c->m_body);
				}
				Remove(EntityList[i]);
			}
		}
	}
	static void Draw(int draw_layer)
	{
		for (Entity* e : EntityList)
		{
			if (e->m_draw_layers >= draw_layer && !e->m_destroy)
			{
				e->Draw(draw_layer);
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
