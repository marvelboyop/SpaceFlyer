#pragma once

#include "raylib.h"

struct MenuAsteroid
{
    Vector2 position;
    Vector2 velocity;

    float radius;

    float rotation;
    float rotationSpeed;

    float wobbleOffset;
    float wobbleStrength;

    float alpha;

    int layer;
};