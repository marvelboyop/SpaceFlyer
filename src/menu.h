#pragma once

#include "raylib.h"

enum GameState
{
    MENU,
    PLAYING,
    GAME_OVER,
    WIN
};

// menuShipState so can use them
static const int BASE_WIDTH = 1280;
static const int BASE_HEIGHT = 720;

// Animation timers
extern float menuT;      // figure-8 path parameter
extern float titlePulse; // title glow oscillation
extern float shipPulse;  // engine flame oscillation (always ticks)
extern float starDrift;  // menu star parallax offset

void InitMenu();
void UpdateMenu(GameState &state, float scale, int offsetX, int offsetY);
void DrawMenu();
void UnloadMenu();