#pragma once
#include "Screens.h"
class BaseScreen
{
public:
	virtual ~BaseScreen() = default;
	virtual void Draw() = 0;
	virtual Screens Update(float dt) = 0;
};