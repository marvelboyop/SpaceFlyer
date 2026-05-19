#include "spaceship_particles.h"

#include "raylib.h"
#include "raymath.h"

#include <vector>
#include <cmath>

struct Particle
{
    Vector2 position;
    Vector2 velocity;

    float life;
};

static std::vector<Particle> menuParticles;

void InitShipParticles()
{
    menuParticles.clear();
}

void UpdateShipParticles(
    float dt,
    Vector2 shipPos,
    float shipAngle)
{
    // --- Update existing particles ---
    for (int i = (int)menuParticles.size() - 1; i >= 0; i--)
    {
        menuParticles[i].position =
            Vector2Add(
                menuParticles[i].position,
                Vector2Scale(
                    menuParticles[i].velocity,
                    dt));

        menuParticles[i].life -= dt * 2.f;

        if (menuParticles[i].life <= 0)
        {
            menuParticles.erase(
                menuParticles.begin() + i);
        }
    }

    // --- Spawn new particles behind ship ---
    for (int i = 0; i < 2; i++)
    {
        float spread =
            GetRandomValue(-20, 20) * DEG2RAD;

        Particle p;

        p.position = shipPos;

        p.velocity = {
            cosf(shipAngle + PI + spread) *
                (float)GetRandomValue(30, 90),

            sinf(shipAngle + PI + spread) *
                (float)GetRandomValue(30, 90)};

        p.life = 1.f;

        menuParticles.push_back(p);
    }
}

void DrawShipParticles()
{
    for (auto &p : menuParticles)
    {
        DrawCircleV(
            p.position,
            3.f,
            Fade(ORANGE, p.life));

        DrawCircleV(
            p.position,
            5.5f,
            Fade(YELLOW, p.life * 0.5f));
    }
}