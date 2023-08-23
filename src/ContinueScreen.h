#pragma once
#include <raylib.h>
#include "BaseScreen.h"
#include "Settings.h"
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

class ContinueScreen : public BaseScreen
{
private:
    int selectedSlot;
    Rectangle slotRectangles[3];
    bool saveFileExists[3];

public:
    ContinueScreen();

    void Draw() override;
    Screens Update(float dt) override;
};
