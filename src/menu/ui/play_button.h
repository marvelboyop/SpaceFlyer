#pragma once

#include "../../core/gamestate.h"

#include "raylib.h"

void InitPlayButton();

void UpdatePlayButton(
    GameState &state,
    float scale,
    int offsetX,
    int offsetY,
    bool showSettings);

void DrawPlayButton();

Rectangle GetPlayButtonBounds();