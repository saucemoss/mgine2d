#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <array>

enum ColliderType {
	SOLID,
	PLAYER,
};

class Collidable
{
public:
	ColliderType m_colliderType;
	std::string m_colliderData;
	std::array<Rectangle*,4> contact;
	Rectangle rectangle;

	bool colliding = false;;
	float x, y;
	float w, h;
	float vx, vy;
	float normalx, normaly;
	Vector2 normals;
	Vector2 contactPoint;
	float contactTime;

	virtual void DrawCollider() = 0;
};

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
	static bool RectSensor(Rectangle& r);


	bool verlapLeft;
	bool overlapRight;
	bool overlapTop;
	bool overlapBottom;

};