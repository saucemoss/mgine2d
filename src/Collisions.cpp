#include "Collisions.h"
#include <algorithm>
#include <vector>
#include <functional>
#include <raymath.h>
#include <limits>

std::vector<Collidable*> CollisionManager::colliders;


void CollisionManager::DrawColliders()
{
	for (Collidable* c : colliders)
	{
		c->DrawCollider();
	}
}

void CollisionManager::Add(Collidable* c)
{
	colliders.push_back(c);
}

void CollisionManager::Remove(Collidable* c)
{
	auto newEnd = std::remove(colliders.begin(), colliders.end(), c);
	colliders.erase(newEnd, colliders.end());
}

bool CollisionManager::PointInRect(const Vector2& point)
{
	for (Collidable* b : colliders)
	{
		if (CheckCollisionPointRec(point, b->rectangle))
		{
			return true;
		}
	}
	return false;
}

bool CollisionManager::RectVsRect(const Rectangle* r1, const Rectangle* r2)
{
	return (r1->x < r2->x + r2->width && r1->x + r1->width > r2->x && r1->y < r2->y + r2->height && r1->y + r1->height > r2->y);
}

bool CollisionManager::RayVsRect(const Vector2& ray_origin, const Vector2& ray_dir, const Rectangle* target, Vector2& contact_point, Vector2& contact_normal, float& t_hit_near)
{
	contact_normal = { 0,0 };
	contact_point = { 0,0 };

	// Cache division
	Vector2 invdir = Vector2Divide({ 1.0f,1.0f },ray_dir);
	Vector2 targetPos = { target->x,target->y };
	Vector2 targetSize = { target->width, target->height };
	// Calculate intersections with rectangle bounding axes
	Vector2 t_near = Vector2Multiply(Vector2Subtract(targetPos, ray_origin), invdir);
	Vector2 t_far = Vector2Multiply(Vector2Subtract(Vector2Add(targetPos, targetSize), ray_origin), invdir);

	if (std::_Is_nan<float>(t_far.y) || std::_Is_nan<float>(t_far.x)) return false;
	if (std::_Is_nan<float>(t_near.y) || std::_Is_nan<float>(t_near.x)) return false;

	// Sort distances
	if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
	if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

	// Early rejection		
	if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

	// Closest 'time' will be the first contact
	t_hit_near = std::max(t_near.x, t_near.y);

	// Furthest 'time' is contact on opposite side of target
	float t_hit_far = std::min(t_far.x, t_far.y);

	// Reject if ray direction is pointing away from object
	if (t_hit_far < 0)
		return false;
	
	// Contact point of collision from parametric line equation
	//contact_point = ray_origin + t_hit_near * ray_dir;
	contact_point = Vector2Add(ray_origin,{t_hit_near* ray_dir.x, t_hit_near* ray_dir.y});

	if (t_near.x > t_near.y)
		if (invdir.x < 0)
			contact_normal = { 1, 0 };
		else
			contact_normal = { -1, 0 };
	else if (t_near.x < t_near.y)
		if (invdir.y < 0)
			contact_normal = { 0, 1 };
		else
			contact_normal = { 0, -1 };

	// Note if t_near == t_far, collision is principly in a diagonal
	// so pointless to resolve. By returning a CN={0,0} even though its
	// considered a hit, the resolver wont change anything.
	return true;
}

bool CollisionManager::DynamicRectVsRect(const Collidable* r_dynamic, const float fTimeStep, const Rectangle& r_static,
	Vector2& contact_point, Vector2& contact_normal, float& contact_time)
{
	// Check if dynamic rectangle is actually moving - we assume rectangles are NOT in collision to start
	if (r_dynamic->vx == 0 && r_dynamic->vy == 0)
		return false;

	// Expand target rectangle by source dimensions
	Rectangle expanded_target;
	expanded_target.x = r_static.x - r_dynamic->w / 2;
	expanded_target.y = r_static.y - r_dynamic->h / 2;
	expanded_target.width = r_static.width + r_dynamic->rectangle.width;
	expanded_target.height = r_static.height + r_dynamic->rectangle.height;

	if (RayVsRect(Vector2Add({ r_dynamic->x,r_dynamic->y }, { r_dynamic->w / 2,r_dynamic->h / 2 }), 
		Vector2{ r_dynamic->vx * fTimeStep,r_dynamic->vy * fTimeStep }, 
		&expanded_target, contact_point, contact_normal, contact_time))
		return (contact_time >= 0.0f && contact_time < 1.0f);
	else
		return false;
}

bool CollisionManager::ResolveDynamicRectVsRect(Collidable* r_dynamic, const float fTimeStep, Rectangle* r_static)
{
	Vector2 contact_point, contact_normal;
	float contact_time = 0.0f;
	if (DynamicRectVsRect(r_dynamic, fTimeStep, *r_static, contact_point, contact_normal, contact_time))
	{
		if (contact_normal.y > 0) r_dynamic->contact[0] = r_static; else nullptr;
		if (contact_normal.x < 0) r_dynamic->contact[1] = r_static; else nullptr;
		if (contact_normal.y < 0) r_dynamic->contact[2] = r_static; else nullptr;
		if (contact_normal.x > 0) r_dynamic->contact[3] = r_static; else nullptr;

		Vector2 velocity = Vector2{ contact_normal.x * std::abs(r_dynamic->vx),contact_normal.y * std::abs(r_dynamic->vy) };

		r_dynamic->vx += velocity.x * (1 - contact_time);
		r_dynamic->vy += velocity.y * (1 - contact_time);

		return true;
	}

	return false;
}

bool CollisionManager::RaycastPro(const Vector2& ray_origin, const Vector2& ray_dir, Vector2& contact_point, Vector2& contact_normal, float& t_hit_near)
{
	for (Collidable* c : colliders)
	{
		if (c->m_colliderTag != PLAYER && RayVsRect(ray_origin, ray_dir, &c->rectangle, contact_point, contact_normal, t_hit_near))
		{
			return true;
		}
	}
	return false;
}

bool CollisionManager::Raycast(const Vector2& ray_origin, const Vector2& ray_dir)
{
	Vector2 cp, cn;
	float t;
	for (Collidable* c : colliders)
	{
		if (c->m_colliderTag != PLAYER && RayVsRect(ray_origin, ray_dir, &c->rectangle, cp, cn, t))
		{
			return true;
		}
	}

	cp = { 0,0 };
	cn = { 0,0 };
	t = 0;
	return false;
}

ColliderTag CollisionManager::GetCollisionTags(Rectangle& r)
{
	for (Collidable* b : CollisionManager::colliders)
	{
		if (b->m_colliderTag != PLAYER)
		{
			if (CheckCollisionRecs(b->rectangle, r))
			{
				return b->m_colliderTag;
			}
		}
	}
}

Collidable* CollisionManager::GetCollisionObject(Rectangle& r)
{
	for (Collidable* b : CollisionManager::colliders)
	{
		if (b->m_colliderTag != PLAYER)
		{
			if (CheckCollisionRecs(b->rectangle, r))
			{
				return b;
			}
		}
	}
}

bool CollisionManager::RectSensor(Rectangle& r)
{
	for (Collidable* b : CollisionManager::colliders)
	{
		if (b->m_colliderTag != PLAYER)
		{
			if (CheckCollisionRecs(b->rectangle, r))
			{
				return true;
			}
		}
	} 
	return false;
}

void CollisionManager::ResolveCollisions(Collidable* c, float fElapsedTime)
{
	// Sort collisions in order of distance
	Vector2 cp, cn;
	float t = 0, min_t = INFINITY;
	std::vector<std::pair<int, float>> z;

	// Work out collision point, add it to vector along with rect ID
	for (size_t i = 0; i < CollisionManager::colliders.size(); i++)
	{
		if (CollisionManager::colliders[i]->m_colliderTag != PLAYER &&
			CollisionManager::colliders[i]->m_colliderTag != LEVEL_PORTAL) {
			if (DynamicRectVsRect(c, fElapsedTime, CollisionManager::colliders[i]->rectangle, cp, cn, t))
			{
				z.push_back({ i, t });
			}
		}
	}

	// Do the sort
	std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
		{
			return a.second < b.second;
		});

	// Now resolve the collision in correct order 
	for (auto j : z)
		ResolveDynamicRectVsRect(c, fElapsedTime, &CollisionManager::colliders[j.first]->rectangle);

	// UPdate the player rectangles position, with its modified velocity
	c->x += c->vx * fElapsedTime;
	c->y += c->vy * fElapsedTime;
}