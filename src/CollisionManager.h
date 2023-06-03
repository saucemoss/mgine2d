#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <array>
#include <memory>
#include <raylib.h>
#include "ColliderTag.h"

class Collidable;

class CollisionManager
{
public:
	static std::vector<Collidable*> colliders;
	static void DrawColliders();
	static void Add(Collidable*);
	static void Remove(Collidable*);
	static bool PointInRect(const Vector2& point);
	static void ResolveCollisions(Collidable* c, float fElapsedTime);
	static bool RectVsRect(const Rectangle* r1, const Rectangle* r2);
	static bool RayVsRect(const Vector2& ray_origin, const Vector2& ray_dir, const Rectangle* target, Vector2& contact_point, Vector2& contact_normal, float& t_hit_near);
	static bool DynamicRectVsRect(const Collidable* r_dynamic, const float fTimeStep, const Rectangle& r_static,
		Vector2& contact_point, Vector2& contact_normal, float& contact_time);
	static bool ResolveDynamicRectVsRect(Collidable* r_dynamic, const float fTimeStep, Rectangle* r_static);
	static bool RaycastPro(const Vector2& ray_origin, const Vector2& ray_dir, Vector2& contact_point, Vector2& contact_normal, float& t_hit_near);
	static bool Raycast(const Vector2& ray_origin, const Vector2& ray_dir);
	static ColliderTag GetCollisionTags(Rectangle& r);
	static Collidable* GetCollisionObject(Rectangle& r);
	static std::vector<Collidable*> GetCollisionObjects(Rectangle& r);
	static bool RectSensor(Rectangle& r);
	static bool IsCollisionWith(ColliderTag m_colliderTag, Rectangle& r);



	bool verlapLeft;
	bool overlapRight;
	bool overlapTop;
	bool overlapBottom;

};