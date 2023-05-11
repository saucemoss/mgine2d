#include "LevelPortal.h"
#include "GameScreen.h"


LevelPortal::LevelPortal(const Rectangle& rect, const ldtk::Entity& entity, const std::string level, std::string to_level, ldtk::IID iid_refernece)
	:
	m_to_level(to_level),
	m_iid_reference(iid_refernece),
	ldtk_entity(entity),
	m_level(level)

{
	rectangle = rect;
	x = rectangle.x;
	y = rectangle.y;
	w = rectangle.width;
	h = rectangle.height;
	m_colliderTag = LEVEL_PORTAL;
	CollisionManager::Add(this);
	EnitityManager::Add(this);
	std::cout << "created portal" << std::endl;

}

LevelPortal::~LevelPortal()
{
	CollisionManager::Remove(this);
	EnitityManager::Remove(this);
	std::cout << "removing portal" << std::endl;
}


void LevelPortal::Draw()
{
}


void LevelPortal::DrawCollider()
{
	if(is_active)
	{
		DrawRectangleLinesEx(rectangle, 1, BLUE);
	}
}

void LevelPortal::Update(float dt)
{
	if (GameScreen::LevelMgr->currentLdtkLevel->name==m_level)
	{
		is_active = true;
	}
	else
	{
		is_active = false;
	}
}

