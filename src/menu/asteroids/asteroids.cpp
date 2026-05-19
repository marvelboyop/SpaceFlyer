#include "asteroids.h"
#include "asteroid_data.h"

#include "raylib.h"
#include "../../core/constants.h"

#include <vector>
#include <cmath>

static Texture2D asteroidTexture;
static std::vector<MenuAsteroid> menuAsteroids;

void InitAsteroids()
{
    asteroidTexture = LoadTexture("assets/ast.png");
    SetTextureFilter(asteroidTexture, TEXTURE_FILTER_BILINEAR);

    for (int i = 0; i < 9; i++)
    {
        MenuAsteroid a;

        a.layer = GetRandomValue(0, 2);

        a.position = {
            (float)GetRandomValue(-200, VIRTUAL_WIDTH + 200),
            (float)GetRandomValue(-100, VIRTUAL_HEIGHT + 100)};

        if (a.layer == 0)
        {
            a.radius = (float)GetRandomValue(12, 22);
            a.velocity = {-18.f, 6.f};
            a.alpha = 0.18f;
        }
        else if (a.layer == 1)
        {
            a.radius = (float)GetRandomValue(22, 38);
            a.velocity = {-35.f, 12.f};
            a.alpha = 0.28f;
        }
        else
        {
            a.radius = (float)GetRandomValue(45, 75);
            a.velocity = {-55.f, 20.f};
            a.alpha = 0.40f;
        }

        a.rotation = (float)GetRandomValue(0, 360);
        a.rotationSpeed = (float)GetRandomValue(-18, 18);
        a.wobbleOffset = (float)GetRandomValue(0, 1000);
        a.wobbleStrength = (float)GetRandomValue(2, 8);

        menuAsteroids.push_back(a);
    }
}

void UpdateAsteroids(float dt)
{
    float t = GetTime();

    for (auto &a : menuAsteroids)
    {
        a.position.x += a.velocity.x * dt;
        a.position.y += a.velocity.y * dt;

        a.position.y += sinf(t + a.wobbleOffset) * a.wobbleStrength * dt;

        a.rotation += a.rotationSpeed * dt;
    }
}

void DrawAsteroids()
{
    Rectangle source = {
        0,
        0,
        (float)asteroidTexture.width,
        (float)asteroidTexture.height};

    for (auto &a : menuAsteroids)
    {
        float size = a.radius * 2.f;

        Rectangle dest = {
            a.position.x,
            a.position.y,
            size,
            size};

        Vector2 origin = {
            size / 2.f,
            size / 2.f};

        DrawTexturePro(
            asteroidTexture,
            source,
            dest,
            origin,
            a.rotation,
            Fade(WHITE, a.alpha));

        DrawCircleV(
            a.position,
            a.radius * 1.2f,
            Fade(BLACK, 0.08f * a.alpha));
    }
}

void UnloadAsteroids()
{
    UnloadTexture(asteroidTexture);
}