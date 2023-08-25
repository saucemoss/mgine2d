#include "BossHook.h"
#include "LevelManager.h"
#include "GameScreen.h"

BossHook::BossHook(const Rectangle& rect, bool going_right) :
	Collidable(rect, b2_kinematicBody, BOSSHOOK),
	going_right(going_right)
{
	m_body->SetBullet(true);
	m_body->SetFixedRotation(true);
	m_fixture->SetSensor(true);
	InitAnimations();
	queue_entity_add = true;
	time_to_live = 5.0f;
}

BossHook::~BossHook()
{
	EnitityManager::Remove(this);
}


void BossHook::Draw(int l)
{
	auto spritePosX = center_pos().x ;
	auto spritePosY = center_pos().y - 16.0f;
	float angle = m_body->GetAngle() * 180 / PI;

	DrawTexturePro(*sprite, {10 * 32.0f, 5* 32.0f, (going_right ? 32.0f : -32.0f), 32.0f },
		Rectangle{ spritePosX,spritePosY,32.0f, 32.0f },
		{ 0,0 },
		angle,
		WHITE);

}

void BossHook::Update(float dt)
{

	time_to_live -= dt;
	if (axed || time_to_live <= 0.0f)
	{
		m_destroy = true;
	}

	m_body->SetLinearVelocity({ 0,0 });
	const float projectileSpeed = 16.0f;

	m_body->SetLinearVelocity({ (going_right ? projectileSpeed : -projectileSpeed), 0.0f });

	ParticleEmitter* p = new ParticleEmitter({pos().x + (going_right ? -16 : 16), pos().y+2});
	GameScreen::Particles->Add(DefinedEmitter::acid_projectile, p);
	p->EmitParticles();

	if (player_in_sensor)
	{
		GameScreen::add_trauma(0.6f);
		ParticleEmitter* p = new ParticleEmitter(pos());
		GameScreen::Particles->Add(DefinedEmitter::acid_explosion, p);
		p->EmitParticles();
		GameScreen::player->take_dmg(10);
		m_destroy = true;
	}


}

void BossHook::InitAnimations()
{
	//11,5
	sprite = TextureLoader::GetTexture("MOTHMAN");
}
