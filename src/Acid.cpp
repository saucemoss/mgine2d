#include "Acid.h"
#include "GameScreen.h"

Acid::Acid(const Rectangle& rectangle)
	:
	Collidable(rectangle, b2_staticBody, ACID)
{
	m_fixture->SetSensor(true);
	EnitityManager::Add(this);
	p_emitter1 = new ParticleEmitter({pos().x - 8, pos().y - 12});
	GameScreen::Particles->Add(DefinedEmitter::acid_bursts, p_emitter1);
	p_emitter2 = new ParticleEmitter({ pos().x + 8, pos().y - 12 });
	GameScreen::Particles->Add(DefinedEmitter::acid_bursts, p_emitter2);
}

Acid::~Acid()
{
	EnitityManager::Remove(this);

}

void Acid::Update(float dt)
{
}

void Acid::Draw(int l)
{
}
