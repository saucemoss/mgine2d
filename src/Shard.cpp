#include "LevelManager.h"
#include "Shard.h"

Shard::Shard(Texture2D* sprite, Rectangle& sprite_rect, Rectangle& pos_rect, float ttl) :
	time_to_live(ttl),
	sprite(sprite),
	sprite_rect(sprite_rect),
	pos_rect(&pos_rect),
	Collidable(pos_rect, b2_dynamicBody, SHARD)
{
	m_fixture->SetDensity(30.0f);
	m_fixture->SetFriction(10.0f);
	m_body->SetFixedRotation(false);
	m_body->SetLinearDamping(10.0f);
	m_body->SetAngularDamping(10.0f);
	m_body->ResetMassData();
	queue_entity_add = true;
}

Shard::~Shard()
{

	EnitityManager::Remove(this);
}


void Shard::Draw(int l)
{
	float angle = m_body->GetAngle() * 180 / PI;

	DrawTexturePro(*sprite,sprite_rect,
		{ pos().x, pos().y, sprite_rect.width, sprite_rect.height },
		{ float(sprite_rect.width / 2.0f),float(sprite_rect.height / 2.0f) },
		angle,
		WHITE);


}

void Shard::Update(float dt)
{
	time_to_live -= dt;

	if (time_to_live <= 0.0f)
	{
		m_destroy = true;
	}
}

