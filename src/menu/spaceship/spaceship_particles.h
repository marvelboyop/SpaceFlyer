#pragma once

#include "raylib.h"

void InitShipParticles();

void UpdateShipParticles(
    float dt,
    Vector2 shipPos,
    float shipAngle);

void DrawShipParticles();