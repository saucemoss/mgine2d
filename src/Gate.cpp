#include "Gate.h"
#include "Shard.h"
#include "SoundManager.h"


Gate::Gate(const Rectangle& rect)
	:
	Collidable(rect, b2_kinematicBody, GATE)
{
	gate_shards = nullptr;
	InitAnimations();
	EnitityManager::Add(this);
}

Gate::~Gate()
{
	delete gate_shards;
	gate_shards = nullptr;
	EnitityManager::Remove(this);
}

void Gate::Draw(int l)
{
	auto spritePosX = pos().x;
	auto spritePosY = pos().y;
	float angle = m_body->GetAngle() * 180 / PI;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ CurrentFrame().width / 2,CurrentFrame().height / 2 },
		angle,
		WHITE);
}

void Gate::Update(float dt)
{
	SwitchFrames(dt);

	if (dmg <= 4)
	{
		FreezeFrame("GATE", dmg);
	}
	else if (!m_destroy)
	{
		Rectangle r = { CurrentFrame().x + 40, CurrentFrame().y + 30, settings::tileSize, settings::tileSize };
		gate_shards = new Shards(sprite, r, pos(), 5, 1.5f, 10.0f);

		m_destroy = true;
		SpawnOrbs(2, center_pos());
		PlaySound(SoundManager::sounds["metal_fall"]);
	}


}

void Gate::InitAnimations()
{

	sprite = TextureLoader::GetTexture("GATE");
	animations->InitializeGateAnimations();
	FreezeFrame("GATE", dmg);

}

void Gate::TakeDmg(int i)
{
	dmg += i;
	PlaySound(SoundManager::sounds["metal_hit"]);
	PlaySound(SoundManager::sounds["axe_solid_hit"]);
	SpawnOrbs(1, center_pos());

}

