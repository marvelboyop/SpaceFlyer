#include "menu.h"
#include "../core/constants.h"
#include "background/nebula.h"
#include "background/stars.h"
#include "asteroids/asteroids.h"
#include "spaceship/spaceship.h"
#include "ui/title.h"
#include "ui/play_button.h"
#include "ui/settings_button.h"
#include "ui/settings_panel.h"
float titlePulse = 0.f;
float shipPulse = 0.f;
float starDrift = 0.f;
static bool showSettings = false;
void InitMenu()
{
    InitStars();
    InitAsteroids();
    InitSpaceship();
    InitPlayButton();
    InitSettingsButton();
}
void UpdateMenu(GameState &state, float scale, int offsetX, int offsetY)
{
    float dt = GetFrameTime();
    titlePulse += dt * 2.0f;
    starDrift += dt * 30.0f;
    UpdateMenuShip(dt);
    UpdateAsteroids(dt);
    UpdatePlayButton(state, scale, offsetX, offsetY, showSettings);
    UpdateSettingsButton(showSettings, scale, offsetX, offsetY);
    if (showSettings && IsKeyPressed(KEY_ESCAPE))
        showSettings = false;
}
void DrawMenu()
{
    DrawNebula();
    DrawStars();
    DrawAsteroids();
    DrawEngineTrail();
    DrawSpaceship();
    DrawTitle();
    DrawPlayButton();
    DrawSettingsButton();
    if (showSettings)
        DrawSettingsPanel();
}
void UnloadMenu()
{
    UnloadAsteroids();
    UnloadSpaceship();
}