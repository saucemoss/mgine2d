#pragma once

#include <raylib.h>
#include "BaseScreen.h"

class TitleScreen : public BaseScreen
{
private:
	Texture2D* texture;

public:
	TitleScreen();
	~TitleScreen();

	void Draw() override;
	Screens Update(float dt) override;
};
