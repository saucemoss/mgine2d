#include "FireAxe.h"
#include "LevelManager.h"

FireAxe::FireAxe(const Rectangle& rect) :
	Collidable(rect, b2_dynamicBody, FIREAXE)
{
	m_body->SetBullet(true);
	m_fixture->SetDensity(20.0f);
	m_fixture->SetFriction(10.0f);
	//m_fixture->SetRestitution(0.05f);
	m_body->SetLinearDamping(3.0f);
	m_body->SetAngularDamping(3.0f);
	
	m_body->ResetMassData();
	InitAnimations();
	EnitityManager::Add(this);
}

FireAxe::~FireAxe()
{
	EnitityManager::Remove(this);
}


void FireAxe::Draw()
{
	auto spritePosX = pos().x-3;
	auto spritePosY = pos().y+3;
	float angle = m_body->GetAngle() * 180 / PI;

	DrawTexturePro(*sprite,
		{
			10 * 32, 7 * 32,settings::tileSize,settings::tileSize
		},
		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
		{ settings::tileSize / 2,settings::tileSize / 2 },
		angle,
		WHITE);

	//float av = fabs(m_body->GetLinearVelocity().x);
	//if (av > axvel)
	//{
	//	axvel = av;
	//}
	//std::string avs = std::to_string(axvel);
	//DrawText(avs.c_str(), 200, 200, 20, GREEN);
}

void FireAxe::Update(float dt)
{
	if (m_destroy)
	{
		LevelManager::world->DestroyBody(m_body);
	}
}

void FireAxe::InitAnimations()
{
	//10,7
	sprite = TextureLoader::GetTexture("MOTHMAN");
}
