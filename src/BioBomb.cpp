#include "BioBomb.h"
#include "LevelManager.h"

BioBomb::BioBomb(const Rectangle& rect) :
	Collidable(rect, b2_dynamicBody, BIOBOMB)
{
	m_body->SetBullet(true);
	m_body->SetFixedRotation(false);
	m_fixture->SetDensity(1.0f);
	m_fixture->SetFriction(1.0f);
	m_fixture->SetRestitution(0.05f);
	m_body->SetLinearDamping(1.0f);
	m_body->SetAngularDamping(1.0f);

	m_body->ResetMassData();
	InitAnimations();
	queue_entity_add = true;
}

BioBomb::~BioBomb()
{
	EnitityManager::Remove(this);
	LevelManager::world->DestroyBody(m_body);
}


void BioBomb::Draw(int l)
{
	auto spritePosX = pos().x ;
	auto spritePosY = pos().y ;
	float angle = m_body->GetAngle() * 180 / PI;

	DrawTexturePro(*sprite, CurrentFrame(),
		Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
		{ float(CurrentFrame().width/2.0f),float(CurrentFrame().height/2.0f) },
		angle,
		WHITE);

}

void BioBomb::Update(float dt)
{
	SwitchFrames(dt);
}

void BioBomb::InitAnimations()
{
	sprite = TextureLoader::GetTexture("HSPIT_1");
	animations->InitializeHeadSpitAnimations();
	SetAnimation("HSPIT_PROJ");
}
