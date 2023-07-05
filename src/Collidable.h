#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <array>
#include <memory>
#include <raylib.h>
#include <box2d.h>
#include "Settings.h"
#include <iostream>
#include <b2_settings.h>

class Collidable;

enum ColliderTag {
	UNASSIGNED,
	SOLID,
	PLAYER,
	LEVEL_PORTAL,
	DOOR,
	ELEVATOR, ELEVATOR_CALL_SW,
	M_BLOCK, M_BLOCK_TOP, W_CRATE,
	INFECTED_H, FIREAXE
};

struct FixtureUserData
{
	std::string name;
};

class Collidable
{
public:
	Collidable(Rectangle rectangle, b2BodyType type, ColliderTag collider_tag)
	{
		m_colliderTag = collider_tag;
		switch (type)
		{
		case b2_dynamicBody:
			switch (collider_tag)
			{
			case PLAYER:
				SetupSimpleBox(rectangle, type, true);
				break;
			case W_CRATE:
				SetupSimpleBox(rectangle, type, false);
				break;
			case FIREAXE:
				SetupSimpleBox(rectangle, type, false);
				break;
			default:
				SetupSimpleBox(rectangle, type, true);
			}
			break;
		case b2_kinematicBody:
			SetupSimpleBox(rectangle, type, true);
			break;
		case b2_staticBody:	
			SetupSimpleBox(rectangle, type, true, 0.0f);
			break;
		}

		m_rectangle = rectangle;

	}

	void SetupSimpleBox(Rectangle& rectangle, b2BodyType type, bool fix_rotate, float density = 1.0f)
	{
		m_box.SetAsBox(float(rectangle.width / 2.0f / settings::PPM),
			float(rectangle.height / 2.0f / settings::PPM));
		m_bodyDef.fixedRotation = fix_rotate;
		m_bodyDef.position.Set((rectangle.x + rectangle.width / 2) / settings::PPM,
			(rectangle.y + rectangle.height / 2) / settings::PPM);
		m_bodyDef.type = type;
		m_bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
		m_body = world->CreateBody(&m_bodyDef);
		m_fixtureDef.friction = 1.0f;
		m_fixtureDef.shape = &m_box;
		m_fixture = m_body->CreateFixture(&m_box, density);
	}

	~Collidable()
	{

	}

	b2Body* m_body = nullptr;
	b2Fixture* m_fixture = nullptr;
	ColliderTag m_colliderTag = UNASSIGNED;
	b2FixtureDef m_fixtureDef;
	b2BodyDef m_bodyDef;
	b2PolygonShape m_box;
	static b2World* world;
	

	void RecreateBody()
	{
		if (m_body != nullptr)
		{
			m_body = world->CreateBody(&m_bodyDef);
			m_fixture = m_body->CreateFixture(&m_box, 1.0f);
		}
	}

	void DrawCollider()
	{

		m_rectangle =
		{
			pos().x - m_rectangle.width / 2,
			pos().y - m_rectangle.height / 2,
			m_rectangle.width,
			m_rectangle.height
		};
		DrawRectangleLinesEx(m_rectangle, 1, RED);
	};

	Rectangle m_rectangle = { 0,0,0,0 };

	Vector2 pos()
	{ 
		if (m_body != nullptr)
		{
			return {
				m_body->GetPosition().x * settings::PPM,
				m_body->GetPosition().y * settings::PPM
			};
		}
		else
		{
			return { 0,0 };
		}
	};

	Vector2 center_pos()
	{
		if (m_body != nullptr)
		{
			return {
				pos().x - m_rectangle.width / 2,
				pos().y - m_rectangle.height / 2,
			};
		}
		else
		{
			return { 0,0 };
		}
	};

private:
	Collidable();
};

