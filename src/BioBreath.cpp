#include "BioBreath.h"
#include "LevelManager.h"
#include "GameScreen.h"

BioBreath::BioBreath(const Rectangle& rect, Vector2 direction) :
	Collidable(rect, b2_dynamicBody, BIOBREATH), direction(direction)
{
	m_body->SetBullet(true);
	m_body->SetFixedRotation(true);
	m_fixture->SetDensity(0.2f);
	m_fixture->SetFriction(2.0f);
	m_fixture->SetRestitution(0.05f);
	m_body->SetLinearDamping(50.0f);
	m_body->SetAngularDamping(1.0f);
	m_fixture->SetSensor(true);
	m_body->ResetMassData();
	InitAnimations();
	queue_entity_add = true;
	time_to_live = 3.0f;

	const float projectileSpeed = 80.0f;

	Vector2 move = Vector2Scale(direction, projectileSpeed);
	b2Vec2 vel = { move.x, move.y };
	m_body->ApplyLinearImpulseToCenter(vel, true);

}

BioBreath::~BioBreath()
{
	EnitityManager::Remove(this);
}


void BioBreath::Draw(int l)
{

}

void BioBreath::Update(float dt)
{
	SwitchFrames(dt);
	time_to_live -= dt;
	if (axed || time_to_live <= 0.0f)
	{
		m_destroy = true;
	}
	ParticleEmitter* p = new ParticleEmitter(pos());
	GameScreen::Particles->Add(DefinedEmitter::breath_proj, p);
	p->EmitParticles();

}

void BioBreath::InitAnimations()
{
	sprite = TextureLoader::GetTexture("HSPIT_1");
	animations->InitializeHeadSpitAnimations();
	SetAnimation("HSPIT_PROJ");
}
