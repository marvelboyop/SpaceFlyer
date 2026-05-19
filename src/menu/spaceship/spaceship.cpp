#include "spaceship.h"
#include "spaceship_particles.h"
#include "../../core/constants.h"
#include "../ui/play_button.h"

#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

#undef min
#undef max

static Texture2D shipTexture;

// Ship patrol state
static Vector2 shipMenuPos = {150.f, 620.f};
static float shipMenuAngle = 0.f;
static int patrolWaypoint = 0;
static float patrolT = 0.f;

// Hover state
static bool isHovering = false;
static float hoverTimer = 0.f;
static float hoverOrbitAngle = 0.f;

static const float HOVER_DURATION = 2.f;
static const float HOVER_RADIUS = 45.f;
static const int HOVER_WAYPOINT = 1;

// Waypoints: bottom-left → settings → bottom-right → top-left
static const Vector2 waypoints[] = {
    {120.f, 620.f},
    {(float)VIRTUAL_WIDTH - 65.f, 55.f},
    {(float)VIRTUAL_WIDTH - 65.f, 620.f},
    {120.f, 55.f},
};

static const int WAYPOINT_COUNT = 4;

extern float shipPulse;

void InitSpaceship()
{
    shipTexture = LoadTexture("assets/Spaceship.png");

    SetTextureFilter(
        shipTexture,
        TEXTURE_FILTER_BILINEAR);

    InitShipParticles();
}

static float LerpAngle(float current, float target, float t)
{
    float diff = target - current;

    while (diff > PI)
        diff -= 2.f * PI;

    while (diff < -PI)
        diff += 2.f * PI;

    return current + diff * t;
}

void UpdateMenuShip(float dt)
{
    UpdateShipParticles(dt, shipMenuPos, shipMenuAngle);

    // --- Hovering around settings btn ---
    if (isHovering)
    {
        hoverTimer += dt;

        hoverOrbitAngle += dt * 2.5f;

        Vector2 centre = waypoints[HOVER_WAYPOINT];

        shipMenuPos.x =
            centre.x + cosf(hoverOrbitAngle) * HOVER_RADIUS;

        shipMenuPos.y =
            centre.y + sinf(hoverOrbitAngle) * HOVER_RADIUS;

        shipMenuAngle =
            LerpAngle(
                shipMenuAngle,
                hoverOrbitAngle + PI / 2.f,
                0.15f);

        if (hoverTimer >= HOVER_DURATION)
        {
            isHovering = false;

            hoverTimer = 0.f;

            patrolWaypoint = HOVER_WAYPOINT;

            patrolT = 0.f;

            shipMenuPos = waypoints[HOVER_WAYPOINT];
        }

        return;
    }

    // --- Patrol between waypoints ---
    int nextWP =
        (patrolWaypoint + 1) % WAYPOINT_COUNT;

    Vector2 from = waypoints[patrolWaypoint];
    Vector2 to = waypoints[nextWP];

    float dx = to.x - from.x;
    float dy = to.y - from.y;

    float segLen =
        sqrtf(dx * dx + dy * dy);

    Vector2 curPos = {
        from.x + dx * patrolT,
        from.y + dy * patrolT};

    auto distTo = [](Vector2 a, Vector2 b)
    {
        return sqrtf(
            (a.x - b.x) * (a.x - b.x) +
            (a.y - b.y) * (a.y - b.y));
    };

    Rectangle playBtn = GetPlayButtonBounds();

    Vector2 playCentre = {
        playBtn.x + playBtn.width / 2.f,
        playBtn.y + playBtn.height / 2.f};

    Vector2 settingsCentre =
        waypoints[HOVER_WAYPOINT];

    float minDist =
        std::min(
            distTo(curPos, settingsCentre),
            distTo(curPos, playCentre));

    float speed =
        (minDist < 150.f)
            ? 60.f + (minDist / 150.f) * 120.f
            : 180.f;

    patrolT += (speed * dt) / segLen;

    if (patrolT >= 1.f)
    {
        if (nextWP == HOVER_WAYPOINT)
        {
            isHovering = true;

            hoverTimer = 0.f;

            shipMenuPos =
                waypoints[HOVER_WAYPOINT];
        }
        else
        {
            patrolWaypoint = nextWP;

            patrolT = 0.f;
        }

        return;
    }

    shipMenuPos.x =
        from.x + dx * patrolT;

    shipMenuPos.y =
        from.y + dy * patrolT;

    shipMenuAngle =
        LerpAngle(
            shipMenuAngle,
            atan2f(dy, dx),
            0.15f);
}

void DrawSpaceship()
{
    float scale = 0.1f;
    float alpha = 1.0f;

    float w = shipTexture.width * scale;
    float h = shipTexture.height * scale;

    Rectangle source = {
        0,
        0,
        (float)shipTexture.width,
        (float)shipTexture.height};

    Rectangle dest = {
        shipMenuPos.x,
        shipMenuPos.y,
        w,
        h};

    Vector2 origin = {
        w / 2.f,
        h / 2.f};

    float angleDeg =
        shipMenuAngle * RAD2DEG + 90.f;

    // Engine nozzle position (behind the ship)
    Vector2 back = {
        shipMenuPos.x - cosf(shipMenuAngle) * (h / 2.f),
        shipMenuPos.y - sinf(shipMenuAngle) * (h / 2.f)};

    // --- Orange flame pulse ---
    float fp =
        0.55f + 0.45f * sinf(shipPulse * 12.f);

    Vector2 flameTip = {
        back.x - cosf(shipMenuAngle) * 38.f * fp,
        back.y - sinf(shipMenuAngle) * 38.f * fp};

    Vector2 flameL = {
        back.x + cosf(shipMenuAngle + PI / 2.f) * 10.f,
        back.y + sinf(shipMenuAngle + PI / 2.f) * 10.f};

    Vector2 flameR = {
        back.x + cosf(shipMenuAngle - PI / 2.f) * 10.f,
        back.y + sinf(shipMenuAngle - PI / 2.f) * 10.f};

    DrawTriangle(
        flameTip,
        flameL,
        flameR,
        Fade(Color{255, 60, 0, 255}, alpha * 0.90f));

    DrawTriangle(
        flameTip,
        flameL,
        flameR,
        Fade(YELLOW, alpha * 0.50f));

    DrawTexturePro(
        shipTexture,
        source,
        dest,
        origin,
        angleDeg,
        Fade(WHITE, alpha));
}

void DrawEngineTrail()
{
    DrawShipParticles();
}

void UnloadSpaceship()
{
    UnloadTexture(shipTexture);
}