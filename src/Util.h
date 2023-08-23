#pragma once
#include <string>
#include <raylib.h>
#include <box2d.h>
#include "Settings.h"


namespace util
{
	struct FixtureUserData
	{
		std::string name;
	};

	static std::string VecToString(Vector2 vec)
	{
		return std::to_string(vec.x) + ", " + std::to_string(vec.y);
	}


	static b2Fixture* SimpleSensor(b2Body* body, std::string name, float width, float height, float pos_x = 0.0f, float pos_y = 0.0f)
	{
		b2PolygonShape shape;
		shape.SetAsBox(width, height, b2Vec2(pos_x, pos_y), 0);
		FixtureUserData* sesnor_name = new FixtureUserData;
		sesnor_name->name = name;
		b2FixtureDef fDef;
		fDef.isSensor = true;
		fDef.shape = &shape;
		fDef.userData.pointer = reinterpret_cast<uintptr_t>(sesnor_name);
		return body->CreateFixture(&fDef);
	}



	static b2Fixture* SimpleCircleSensor(b2Body* body, std::string name, float radius, float pos_x = 0.0f, float pos_y = 0.0f)
	{
		b2CircleShape shape;
		shape.m_p.Set(pos_x / settings::PPM, pos_y / settings::PPM);
		shape.m_radius = radius / settings::PPM;
		FixtureUserData* sesnor_name = new FixtureUserData;
		sesnor_name->name = name;
		b2FixtureDef fDef;
		fDef.isSensor = true;
		fDef.shape = &shape;
		fDef.userData.pointer = reinterpret_cast<uintptr_t>(sesnor_name);
		return body->CreateFixture(&fDef);
	}
}


