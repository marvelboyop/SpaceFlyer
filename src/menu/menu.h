#pragma once
#include "../core/gamestate.h"
extern float titlePulse;
extern float shipPulse;
extern float starDrift;
void InitMenu();
void UpdateMenu(GameState &state, float scale, int offsetX, int offsetY);
void DrawMenu();
void UnloadMenu();

// menuShipState so can use them
// static const int BASE_WIDTH = 1280;
// static const int BASE_HEIGHT = 720;