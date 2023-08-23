#include "raylib.h"
#include "Game.h"
#include "Settings.h"


int main(void)
{
    Game game{ settings::screenWidth,
                settings::screenHeight,
                settings::fps,
                settings::windowTitle };

    while (!WindowShouldClose() && !game.exit_window)
    {
        game.Tick();
    }
    CloseWindow();
    return 0;
}
