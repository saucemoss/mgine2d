#include "WoodCrate.h"
#include "Shard.h"
#include "SoundManager.h"


WoodCrate::WoodCrate(const Rectangle& rect)
	:
	Collidable(rect, b2_dynamicBody, W_CRATE)
{
	crate_shards = nullptr;
	m_fixture->SetDensity(8.0f);
	m_body->ResetMassData();
	InitAnimations();
	EnitityManager::Add(this);
}

WoodCrate::~WoodCrate()
{
	delete crate_shards;
	crate_shards = nullptr;
	EnitityManager::Remove(this);
}

void WoodCrate::Draw(int l)
{
	auto spritePosX = pos().x;
	auto spritePosY = pos().y - 2;
	float angle = m_body->GetAngle() * 180 / PI;

	DrawTexturePro(*sprite,
		CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
		{ settings::tileSize / 2,settings::tileSize / 2 },
		angle,
		WHITE);
}

void WoodCrate::Update(float dt)
{
	SwitchFrames(dt);

	if (dmg <= 5)
	{
		FreezeFrame("W_CRATE", dmg);
	}
	else if(!m_destroy)
	{
		crate_shards = new Shards(sprite, CurrentFrame(), pos(), 7, 1.5f, 50.0f);
		
		m_destroy = true;
		PlaySound(SoundManager::sounds["crate_break"]);
	}


	m_rectangle =
	{
		pos().x - m_rectangle.width / 2,
		pos().y - m_rectangle.height / 2,
		m_rectangle.width,
		m_rectangle.height
	};
}

void WoodCrate::InitAnimations()
{
	//0,19
	//sprite = TextureLoader::GetTexture("MOTHMAN");
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeDecorAnimations();
	FreezeFrame("W_CRATE", dmg);

}

void WoodCrate::TakeDmg(int i)
{
	dmg += i;
	std::string dmgs[] = { "woodhit1","woodhit2","woodhit3" };
	SoundManager::PlayRandSounds(dmgs, 3);

}

