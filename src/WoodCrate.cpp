#include "WoodCrate.h"

WoodCrate::WoodCrate(const Rectangle& rect)
	:
	Collidable(rect, b2_dynamicBody, W_CRATE)
{
	
	InitAnimations();
	EnitityManager::Add(this);
}

WoodCrate::~WoodCrate()
{
	EnitityManager::Remove(this);
}

void WoodCrate::Draw()
{
	auto spritePosX = pos().x;
	auto spritePosY = pos().y;
	float angle = m_body->GetAngle() * 180 / PI;

	DrawTexturePro(*sprite,
		{
			19 * 32,0,settings::tileSize,settings::tileSize
		},
		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
		{ settings::tileSize/2,settings::tileSize/2 },
		angle,
		WHITE);
}

void WoodCrate::Update(float dt)
{
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
	sprite = TextureLoader::GetTexture("MOTHMAN");

}

