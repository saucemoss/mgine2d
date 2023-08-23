#pragma once

#include <raylib.h>
#include "BaseScreen.h"

class TitleScreen : public BaseScreen
{
private:

public:
	TitleScreen();
	~TitleScreen();
	bool key_pressed = false;

	void Draw() override;
	Screens Update(float dt) override;
};
