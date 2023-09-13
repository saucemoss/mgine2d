#include "EnergyOrb.h"
#include "LevelManager.h"
#include "GameScreen.h"

EnergyOrb::EnergyOrb(const Rectangle& rect) :
	Collidable(rect, b2_kinematicBody, EORB)
{
	m_body->SetBullet(true);
	m_body->SetFixedRotation(true);
	m_fixture->SetDensity(1.0f);
	m_fixture->SetFriction(1.0f);
	m_fixture->SetRestitution(0.05f);
	m_body->SetLinearDamping(1.0f);
	m_body->SetAngularDamping(1.0f);
	m_fixture->SetSensor(true);
	m_body->ResetMassData();
	float m_size_min = -16.0f;
	float m_size_max = 16.0f;
	float posX = m_size_min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_size_max - m_size_min)));
	float posY = m_size_min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_size_max - m_size_min)));
	random_spread_impulse = { posX,posY };
	InitAnimations();
	queue_entity_add = true;
}

EnergyOrb::~EnergyOrb()
{
	EnitityManager::Remove(this);
}


void EnergyOrb::Draw(int l)
{

}

void EnergyOrb::Update(float dt)
{
	SwitchFrames(dt);

	m_body->SetLinearVelocity({ 0,0 });
	target = GameScreen::player->center_pos();
	const float projectileSpeed = 8.0f;


	if (Vector2Distance(center_pos(), target) > 64)
	{
		ParticleEmitter* p = new ParticleEmitter(pos());
		GameScreen::Particles->Add(DefinedEmitter::energy_orb, p);
		p->EmitParticles();
		time_to_live -= dt;
		if (time_to_live <= 0.0f)
		{
			m_destroy = true;
		}
		return;
	}

	Vector2 direction = Vector2Subtract(target, center_pos());
	direction = Vector2Normalize(direction);
	random_spread_impulse = Vector2Scale(random_spread_impulse, 0.90f);
	Vector2 move = Vector2Add(Vector2Scale(direction, projectileSpeed), random_spread_impulse);
	b2Vec2 vel = { move.x, move.y };
	m_body->SetLinearVelocity(vel);

	ParticleEmitter* p = new ParticleEmitter(pos());
	GameScreen::Particles->Add(DefinedEmitter::energy_orb, p);
	p->EmitParticles();

	if (Vector2Distance(target, center_pos()) <= 2.0f)
	{
		m_destroy = true;
		if (GameScreen::player->current_energy < GameScreen::player->m_max_energy)
		{
			GameScreen::player->current_energy++;
		}
		std::string orbs[] = { "energy_orb1","energy_orb2","energy_orb3"};
		SoundManager::PlayRandSounds(orbs, 3);
	}


}

void EnergyOrb::InitAnimations()
{
	sprite = TextureLoader::GetTexture("HSPIT_1");
	animations->InitializeHeadSpitAnimations();
	SetAnimation("HSPIT_PROJ");
}
