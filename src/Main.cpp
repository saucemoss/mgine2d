#include "raylib.h"
#include "Game.h"
#include "Settings.h"


int main(void)
{
    Game game{ settings::screenWidth,
                settings::screenHeight,
                settings::fps,
                settings::windowTitle };

    while (!game.GameShouldClose())
    {
        game.Tick();
    }

    return 0;
}
