#include "BioBomb.h"
#include "LevelManager.h"
#include "GameScreen.h"

BioBomb::BioBomb(const Rectangle& rect) :
	Collidable(rect, b2_kinematicBody, BIOBOMB)
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
	InitAnimations();
	queue_entity_add = true;
	time_to_live = 5.0f;
	float m_size_min = -16.0f;
	float m_size_max = 16.0f;
	float posX = m_size_min / 2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_size_max / 2 - m_size_min / 4)));
	float posY = m_size_min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_size_max - m_size_min)));
	random_spread_impulse = { posX,posY };
}

BioBomb::~BioBomb()
{
	EnitityManager::Remove(this);
}


void BioBomb::Draw(int l)
{
	//auto spritePosX = pos().x ;
	//auto spritePosY = pos().y ;
	//float angle = m_body->GetAngle() * 180 / PI;

	//DrawTexturePro(*sprite, CurrentFrame(),
	//	Rectangle{ spritePosX,spritePosY,CurrentFrame().width,CurrentFrame().height },
	//	{ float(CurrentFrame().width/2.0f),float(CurrentFrame().height/2.0f) },
	//	angle,
	//	WHITE);

}

void BioBomb::Update(float dt)
{
	SwitchFrames(dt);
	time_to_live -= dt;
	if (axed || time_to_live <= 0.0f)
	{
		m_destroy = true;
	}

	m_body->SetLinearVelocity({0,0});
	target = GameScreen::player->center_pos();
	const float projectileSpeed = 4.0f;

	Vector2 direction = Vector2Subtract(target, center_pos());
	direction = Vector2Normalize(direction);
	random_spread_impulse = Vector2Scale(random_spread_impulse, 0.90f);
	Vector2 move = Vector2Add(Vector2Scale(direction, projectileSpeed), random_spread_impulse);
	b2Vec2 vel = { move.x, move.y };
	m_body->SetLinearVelocity(vel);

	ParticleEmitter* p = new ParticleEmitter(pos());
	GameScreen::Particles->Add(DefinedEmitter::acid_projectile, p);
	p->EmitParticles();
	
	if (Vector2Distance(target, center_pos()) <= 2.0f)
	{
		ParticleEmitter* p = new ParticleEmitter(pos());
		GameScreen::Particles->Add(DefinedEmitter::acid_explosion, p);
		p->EmitParticles();
		GameScreen::player->take_dmg(10);
		m_destroy = true;
	}


}

void BioBomb::InitAnimations()
{
	sprite = TextureLoader::GetTexture("HSPIT_1");
	animations->InitializeHeadSpitAnimations();
	SetAnimation("HSPIT_PROJ");
}
