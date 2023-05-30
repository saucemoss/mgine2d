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
	ldtk::IID m_ldtkID;

    Vector2 GetPos()    {
        return pos;
    }
    void SetPos(Vector2 pos_in)
    {
        pos = pos_in;
    }
    void SetPos(float x, float y)
    {
        pos = { x,y };
    }
protected:
    Vector2 pos;
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
	}
	static void Draw(int draw_layer)
	{
		for (Entity* e : EntityList)
		{
			if (e->m_draw_layer == draw_layer)
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
