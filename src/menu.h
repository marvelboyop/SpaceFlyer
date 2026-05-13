#pragma once

#include "raylib.h"

enum GameState
{
    MENU,
    PLAYING,
    GAME_OVER,
    WIN
};

void InitMenu();
void UpdateMenu(GameState &state, float scale, int offsetX, int offsetY);
void DrawMenu();
void UnloadMenu();