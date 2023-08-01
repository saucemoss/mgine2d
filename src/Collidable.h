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
#include "Util.h"


enum ColliderTag {
	UNASSIGNED,
	ENEMY, NPC,
	SOLID,
	PLAYER,
	LEVEL_PORTAL,
	DOOR,
	ELEVATOR, ELEVATOR_CALL_SW,
	M_BLOCK, M_BLOCK_TOP, W_CRATE,
	INFECTED_H, FLYING_INF, RIBBS, LEGGY, FOOTB, HSPIT,
	FIREAXE, BIOBOMB
};

enum ContactGroup {
	UNASSIGNED_GROUP, PLAYER_GROUP, ENEMY_GROUP, SOLID_GROUP, INTERRACTABLE_GROUP
};

struct FixtureUserData
{
	std::string name;
	ContactGroup tag;
};

class Collidable
{
public:
	b2Body* m_body = nullptr;
	b2Fixture* m_fixture = nullptr;
	ColliderTag m_colliderTag = UNASSIGNED;
	b2FixtureDef m_fixtureDef;
	b2BodyDef m_bodyDef;
	b2PolygonShape m_box;
	b2CircleShape m_circle;
	static b2World* world;

	Collidable(const b2Vec2* verices, int vertices_count, b2BodyType type, ColliderTag collider_tag, const Rectangle& rectangle)
	{
		m_colliderTag = collider_tag;
		m_rectangle = rectangle;
		SetupPolyBody(verices, vertices_count, type, true);
	}

	Collidable(Rectangle rectangle, b2BodyType type, ColliderTag collider_tag)
	{
		m_colliderTag = collider_tag;
		switch (type)
		{
		case b2_dynamicBody:
			switch (collider_tag)
			{
			case PLAYER:
				SetupPolyBody(rectangle, type, true);
				break;
			case W_CRATE:
				SetupSimpleBox(rectangle, type, false);
				break;
			case FIREAXE:
				SetupSimpleBox(rectangle, type, false);
				break;
			case FLYING_INF:
				SetupSimpleCircle(rectangle, type, false);
				break;
			case BIOBOMB:
				SetupSimpleCircle(rectangle, type, false);
				break;
			default:
				SetupSimpleBox(rectangle, type, true);
				break;
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
		m_box.SetAsBox(	float(rectangle.width / 2.0f / settings::PPM),
						float(rectangle.height / 2.0f / settings::PPM));
		m_bodyDef.position.Set(	(rectangle.x + rectangle.width / 2) / settings::PPM,
								(rectangle.y + rectangle.height / 2) / settings::PPM);
		m_bodyDef.fixedRotation = fix_rotate;
		m_bodyDef.type = type;
		m_bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
		m_body = world->CreateBody(&m_bodyDef);
		m_fixtureDef.friction = 1.0f;
		m_fixtureDef.shape = &m_box;
		m_fixture = m_body->CreateFixture(&m_box, density);
	}

	void SetupSimpleCircle(Rectangle& rectangle, b2BodyType type, bool fix_rotate, float density = 1.0f)
	{

		m_circle.m_radius = float(rectangle.width / 2.0f / settings::PPM);
		m_circle.m_p.Set(0,0);
		m_bodyDef.position.Set((rectangle.x + rectangle.width / 2) / settings::PPM,
			(rectangle.y + rectangle.height / 2) / settings::PPM);
		m_bodyDef.fixedRotation = true;
	
		m_bodyDef.type = type;
		m_bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
		m_body = world->CreateBody(&m_bodyDef);
		m_fixtureDef.friction = 1.0f;
		m_fixtureDef.shape = &m_circle;
		m_fixture = m_body->CreateFixture(&m_circle, density);
	}

	void SetupPolyBody(Rectangle& rectangle, b2BodyType type, bool fix_rotate, float density = 1.0f)
	{
		//test polygon collider
		b2PolygonShape poly_body_shape;
		b2Vec2 verices[6] = { {-0.4f,-0.8f},{0.4f,-0.8f},{0.4f,0.4f},{0.3f,0.6f},{-0.3f,0.6f},{-0.4f,0.4f}};
		m_box.Set(verices, 6);
		m_bodyDef.position.Set((rectangle.x + rectangle.width / 2) / settings::PPM,
			(rectangle.y + rectangle.height / 2) / settings::PPM);

		m_bodyDef.fixedRotation = fix_rotate;
		m_bodyDef.type = type;
		m_bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
		m_body = world->CreateBody(&m_bodyDef);
		m_fixtureDef.friction = 1.0f;
		m_fixtureDef.shape = &m_box;
		m_fixture = m_body->CreateFixture(&m_box, density);
	}

	void SetupPolyBody(const b2Vec2* verices, const int vertices_count, b2BodyType type, bool fix_rotate, float density = 1.0f)
	{
		//test polygon collider
		b2PolygonShape poly_body_shape;
		m_box.Set(verices, vertices_count);
		//m_bodyDef.position.Set(poly_body_shape.m_centroid.x, poly_body_shape.m_centroid.y);

		m_bodyDef.position.Set((m_rectangle.x + m_rectangle.width / 2) / settings::PPM,
			(m_rectangle.y + m_rectangle.height / 2) / settings::PPM);

		m_bodyDef.fixedRotation = fix_rotate;
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

	void RecreateBody()
	{
		m_body = world->CreateBody(&m_bodyDef);

		if (m_fixtureDef.shape->m_type==b2Shape::e_polygon)
		{
			m_fixture = m_body->CreateFixture(&m_box, 1.0f);
		}
		else
		{
			m_fixture = m_body->CreateFixture(&m_circle, 1.0f);
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

