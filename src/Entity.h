#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>

class Entity
{
public:
    virtual ~Entity() = default;

    virtual void Draw() = 0;
    virtual void Update(float dt) = 0;

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
	static void Draw()
	{
		for (Entity* e : EntityList)
		{
			e->Draw();
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
