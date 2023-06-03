#pragma once
#include "Entity.h"
#include "Collidable.h"
#include "Animations.h"



class MovingBlock : public Entity, public Collidable, public Animated
{
public:
	MovingBlock(const Rectangle& rect, const ldtk::ArrayField<ldtk::IntPoint> path_points);
	~MovingBlock();
	Rectangle m_top_part;
	// Inherited via Entity
	virtual void Draw() override;
	virtual void Update(float dt) override;

	// Inherited via Collidable
	virtual void DrawCollider() override;

	// Inherited via Animated
	virtual void InitAnimations() override;

private:
	const ldtk::ArrayField<ldtk::IntPoint> m_path_points;
	float speed = 2.2f;
	ldtk::IntPoint m_next_point;
	int m_path_step_counter = 0;
	

};