#include "LevelPortal.h"
#include "GameScreen.h"


LevelPortal::LevelPortal(const Rectangle& rect, std::string to_level, ldtk::IID iid_refernece)
	: Collidable(rect, b2_staticBody),
	m_to_level(to_level),
	m_iid_reference(iid_refernece)
{
	m_colliderTag = LEVEL_PORTAL;
	m_rectangle = rect;
	m_fixture->SetSensor(true);
	EnitityManager::Add(this);

}

LevelPortal::~LevelPortal()
{
	EnitityManager::Remove(this);
}


void LevelPortal::Draw()
{
}


void LevelPortal::Update(float dt)
{

}

