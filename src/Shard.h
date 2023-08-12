#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"
#include "LevelManager.h"

class Shard : public Entity, public Collidable
{
public:
	Shard(Texture2D* sprite, Rectangle& sprite_rect, Rectangle& pos_rect, float ttl);
	~Shard();
	Texture2D* sprite;
	Rectangle sprite_rect;
	Rectangle* pos_rect;
	float time_to_live;
	// Inherited via Entity
	virtual void Draw(int l) override;
	virtual void Update(float dt) override;
	void explode(Vector2 force);
};


class Shards
{
public:
	Shards(Texture2D* sprite, Rectangle rect_in, Vector2 center_pos, int shard_number, float time_to_live, float max_force)
		:
		rect(rect_in)
	{

		std::vector<Rectangle> recs;

		for (int i = 0; i < shard_number; i++)
		{
			recs.push_back({ 
				rect.x + (rect.width / shard_number * i),
				rect.y, 
				rect.width / shard_number,
				rect.height / 2 });
		}

		for (Rectangle r : recs)
		{
			Rectangle pos = { center_pos.x, center_pos.y, rect.width / shard_number, rect.height / shard_number };
			LevelManager::level_entities_safe.push_back(std::make_unique<Shard>(sprite, 
				r, pos, time_to_live));
			Collidable* c = dynamic_cast<Shard*>(LevelManager::level_entities_safe.back().get());

			float randx = -max_force + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_force)));
			float randy = 0.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-max_force)));

			const b2Vec2 impulse = { randx, randy };
			c->m_body->ApplyLinearImpulseToCenter(impulse, true);
			//c->m_body->SetEnabled(false);
		}

	}

	//base texture to be sharded
	Texture2D* sprite;
	Rectangle rect;
	
	float time_to_live;
	float explosion_force;
	int shard_num;

};
