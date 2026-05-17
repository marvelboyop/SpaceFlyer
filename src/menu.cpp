#include "menu.h"
#include "raymath.h"
#include <cmath>
#include <vector>
#include <algorithm>

static const int VIRTUAL_WIDTH = 1280;
static const int VIRTUAL_HEIGHT = 720;

//-------------Particles---------------------
struct Particle
{
    Vector2 position, velocity;
    float life;
};

static std::vector<Particle> menuParticles;

//-----------------Ship-----------------------
static Texture2D shipTexture;

// ---------------- MENU DATA ----------------

static std::vector<Vector2> menuStars;

static Rectangle playBtn = {500, 510, 280, 90};

static bool showSettings = false;

static Rectangle settingsBtn = {
    VIRTUAL_WIDTH - 90,
    30,
    50,
    50};

static Texture2D settingsIcon;
static Texture2D asteroidTexture;

// ---------------- ASTEROIDS ----------------

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

static std::vector<MenuAsteroid> menuAsteroids;

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

// Animation timers
float titlePulse = 0.f;
float shipPulse = 0.f;
float starDrift = 0.f;

// ---------------- HELPER FUNCTIONS ----------------

static void DrawNebula()
{
    DrawCircleV({150.f, 120.f}, 130.f, Fade(Color{58, 22, 110, 255}, 0.35f));
    DrawCircleV({670.f, 480.f}, 160.f, Fade(Color{92, 36, 140, 255}, 0.28f));
    DrawCircleV({400.f, 300.f}, 220.f, Fade(Color{25, 18, 70, 255}, 0.40f));
    DrawCircleV({750.f, 150.f}, 100.f, Fade(Color{140, 40, 120, 255}, 0.22f));
    DrawCircleV({80.f, 400.f}, 80.f, Fade(Color{30, 90, 120, 255}, 0.18f));
    DrawCircleV({980.f, 420.f}, 220.f, Fade(Color{58, 22, 110, 255}, 0.35f));
}

static void DrawStars()
{
    for (auto &st : menuStars)
    {
        float sx = fmodf(
            st.x - starDrift * 0.25f + VIRTUAL_WIDTH,
            (float)VIRTUAL_WIDTH);

        DrawPixelV(Vector2{sx, st.y}, Color{180, 200, 255, 200});
    }
}

// ---------------- ASTEROID DRAW ----------------

static void DrawAsteroids()
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

        // subtle cinematic shadow/glow
        DrawCircleV(
            a.position,
            a.radius * 1.2f,
            Fade(BLACK, 0.08f * a.alpha));
    }
}

static void DrawSpaceship(Vector2 pos, float angle, float scale, float alpha)
{
    float w = shipTexture.width * scale;
    float h = shipTexture.height * scale;
    float s = scale * 8.f;

    Rectangle source = {0, 0, (float)shipTexture.width, (float)shipTexture.height};
    Rectangle dest = {pos.x, pos.y, w, h};
    Vector2 origin = {w / 2.f, h / 2.f};
    float angleDeg = angle * RAD2DEG + 90.f;

    // Engine nozzle position (behind the ship)
    Vector2 back = {
        pos.x - cosf(angle) * (h / 2.f),
        pos.y - sinf(angle) * (h / 2.f)};

    // --- Orange flame pulse ---
    float fp = 0.55f + 0.45f * sinf(shipPulse * 12.f);

    Vector2 flameTip = {
        back.x - cosf(angle) * 38.f * fp,
        back.y - sinf(angle) * 38.f * fp};

    Vector2 flameL = {
        back.x + cosf(angle + PI / 2.f) * 10.f,
        back.y + sinf(angle + PI / 2.f) * 10.f};

    Vector2 flameR = {
        back.x + cosf(angle - PI / 2.f) * 10.f,
        back.y + sinf(angle - PI / 2.f) * 10.f};

    // 1. Orange outer flame
    DrawTriangle(flameTip, flameL, flameR, Fade(Color{255, 60, 0, 255}, alpha * 0.90f));
    DrawTriangle(flameTip, flameL, flameR, Fade(YELLOW, alpha * 0.50f));

    // 2. Oval engine glow
    for (int i = 0; i < 3; i++)
    {
        float t = (float)i / 8.f;

        Vector2 p = {
            back.x + cosf(angle + PI) * 10.f * t * s,
            back.y + sinf(angle + PI) * 10.f * t * s};

        float radius = (10.f - 5.f * t) * scale * 8.f;

        DrawCircleV(p, radius, Fade(ORANGE, alpha * 0.85f * (1.f - t * 0.4f)));
        DrawCircleV(p, radius * 0.9f, Fade(YELLOW, alpha * 0.3f * (1.f - t * 0.5f)));
    }

    // 4. Ship texture on top
    DrawTexturePro(shipTexture, source, dest, origin, angleDeg, Fade(WHITE, alpha));
}

static void DrawTitle()
{
    float pulse = 0.85f + 0.15f * sinf(titlePulse);

    Color titleCol = {
        (unsigned char)(70.f * pulse),
        (unsigned char)(170.f * pulse),
        255,
        255};

    DrawText("Space Flyer", VIRTUAL_WIDTH / 2 - 272, VIRTUAL_HEIGHT / 2 - 173, 90, Fade(SKYBLUE, 0.30f));
    DrawText("Space Flyer", VIRTUAL_WIDTH / 2 - 270, VIRTUAL_HEIGHT / 2 - 170, 90, titleCol);

    DrawText("Precision is Survival.",
             VIRTUAL_WIDTH / 2 - 120,
             VIRTUAL_HEIGHT / 2 - 68,
             24,
             Color{112, 128, 144, 200});

    DrawText("[Arrow Keys] steer     [F] Fullscreen",
             VIRTUAL_WIDTH / 2 - 160,
             VIRTUAL_HEIGHT / 2 + 20,
             18,
             Color{211, 211, 211, 155});

    float enterAlpha = 0.55f + 0.45f * sinf(titlePulse * 2.0f);

    DrawText("PRESS ENTER TO START",
             VIRTUAL_WIDTH / 2 - 133,
             VIRTUAL_HEIGHT / 2 - 20,
             20,
             Fade(WHITE, enterAlpha));
}

static void DrawPlayButton()
{
    Vector2 mouse = GetMousePosition();
    float scaleX = (float)GetRenderWidth() / VIRTUAL_WIDTH;
    float scaleY = (float)GetRenderHeight() / VIRTUAL_HEIGHT;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
    int drawW = (int)(VIRTUAL_WIDTH * scale);
    int drawH = (int)(VIRTUAL_HEIGHT * scale);
    int offsetX = (GetRenderWidth() - drawW) / 2;
    int offsetY = (GetRenderHeight() - drawH) / 2;

    mouse.x = (mouse.x - offsetX) / scale;
    mouse.y = (mouse.y - offsetY) / scale;

    bool hovering = CheckCollisionPointRec(mouse, playBtn);

    Color btn = hovering ? Color{35, 70, 140, 255} : Color{22, 38, 74, 255};
    Color glow = hovering ? Fade(Color{80, 180, 255, 255}, 0.35f) : Fade(Color{80, 180, 255, 255}, 0.18f);

    DrawRectangleRounded({playBtn.x - 4, playBtn.y - 4, playBtn.width + 8, playBtn.height + 8}, 0.30f, 8, glow);
    DrawRectangleRounded(playBtn, 0.28f, 8, btn);
    DrawRectangleRounded({playBtn.x, playBtn.y, playBtn.width, playBtn.height / 2}, 0.28f, 8, Fade(WHITE, 0.05f));

    const char *text = "PLAY";
    int fontSize = 42;
    int textWidth = MeasureText(text, fontSize);
    int textX = playBtn.x + (playBtn.width - textWidth) / 2;
    int textY = playBtn.y + (playBtn.height - fontSize) / 2;

    Color textGlow = hovering ? Fade(SKYBLUE, 0.55f) : Fade(SKYBLUE, 0.35f);

    DrawText(text, textX + 2, textY + 2, fontSize, textGlow);
    DrawText(text, textX, textY, fontSize, Color{220, 240, 255, 255});
}

static void DrawSettingsButton()
{
    Vector2 mouse = GetMousePosition();
    float scaleX = (float)GetRenderWidth() / VIRTUAL_WIDTH;
    float scaleY = (float)GetRenderHeight() / VIRTUAL_HEIGHT;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
    int drawW = (int)(VIRTUAL_WIDTH * scale);
    int drawH = (int)(VIRTUAL_HEIGHT * scale);
    int offsetX = (GetRenderWidth() - drawW) / 2;
    int offsetY = (GetRenderHeight() - drawH) / 2;

    mouse.x = (mouse.x - offsetX) / scale;
    mouse.y = (mouse.y - offsetY) / scale;

    bool hoveringOnSettings = CheckCollisionPointRec(mouse, settingsBtn);

    Color settingsColor = hoveringOnSettings
                              ? Color{50, 90, 170, 255}
                              : Color{25, 45, 85, 255};

    DrawRectangleRounded(settingsBtn, 0.50f, 8, settingsColor);

    float desiredSize = 40.0f;
    float iconScale = desiredSize / settingsIcon.width;
    float finalWidth = settingsIcon.width * iconScale;
    float finalHeight = settingsIcon.height * iconScale;

    Vector2 iconPos = {
        settingsBtn.x + (settingsBtn.width - finalWidth) / 2,
        settingsBtn.y + (settingsBtn.height - finalHeight) / 2};

    DrawTextureEx(settingsIcon, iconPos, 0.0f, iconScale, WHITE);
}

static void DrawSettingsPanel()
{
    DrawRectangleRounded({340, 120, 600, 420}, 0.04f, 10, BLACK);
    DrawRectangleRoundedLinesEx({340, 120, 600, 420}, 0.04f, 10, 2, Fade(SKYBLUE, 0.5f));

    DrawText("SETTINGS", 535, 150, 40, SKYBLUE);
    DrawText("Controls", 420, 240, 28, WHITE);
    DrawText("Arrow Keys -> Move", 420, 290, 22, LIGHTGRAY);
    DrawText("F -> Fullscreen", 420, 330, 22, LIGHTGRAY);
    DrawText("ENTER -> Start Game", 420, 370, 22, LIGHTGRAY);
    DrawText("ESC -> Close Settings", 420, 410, 22, LIGHTGRAY);
}

static void DrawEngineTrail()
{
    for (auto &p : menuParticles)
    {
        DrawCircleV(p.position, 3.f, Fade(ORANGE, p.life));
        DrawCircleV(p.position, 5.5f, Fade(YELLOW, p.life * 0.5f));
    }
}

// ---------------- SHIP UPDATE ----------------

static void UpdateMenuShip(float dt)
{
    // --- Update existing particles ---
    for (int i = (int)menuParticles.size() - 1; i >= 0; i--)
    {
        menuParticles[i].position = Vector2Add(
            menuParticles[i].position,
            Vector2Scale(menuParticles[i].velocity, dt));
        menuParticles[i].life -= dt * 2.f;
        if (menuParticles[i].life <= 0)
            menuParticles.erase(menuParticles.begin() + i);
    }

    // --- Spawn new particles behind ship ---
    for (int i = 0; i < 2; i++)
    {
        float spread = GetRandomValue(-20, 20) * DEG2RAD;
        Particle p;
        p.position = shipMenuPos;
        p.velocity = {
            cosf(shipMenuAngle + PI + spread) * (float)GetRandomValue(30, 90),
            sinf(shipMenuAngle + PI + spread) * (float)GetRandomValue(30, 90)};
        p.life = 1.f;
        menuParticles.push_back(p);
    }

    // --- Angle lerp helper (wraps correctly across ±PI) ---
    auto lerpAngle = [](float current, float target, float t) -> float
    {
        float diff = target - current;
        while (diff > PI)
            diff -= 2.f * PI;
        while (diff < -PI)
            diff += 2.f * PI;
        return current + diff * t;
    };

    // --- Hovering around settings btn ---
    if (isHovering)
    {
        hoverTimer += dt;
        hoverOrbitAngle += dt * 2.5f;

        Vector2 centre = waypoints[HOVER_WAYPOINT];
        shipMenuPos.x = centre.x + cosf(hoverOrbitAngle) * HOVER_RADIUS;
        shipMenuPos.y = centre.y + sinf(hoverOrbitAngle) * HOVER_RADIUS;
        shipMenuAngle = lerpAngle(shipMenuAngle, hoverOrbitAngle + PI / 2.f, 0.15f);

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
    int nextWP = (patrolWaypoint + 1) % WAYPOINT_COUNT;
    Vector2 from = waypoints[patrolWaypoint];
    Vector2 to = waypoints[nextWP];
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    float segLen = sqrtf(dx * dx + dy * dy);

    Vector2 curPos = {from.x + dx * patrolT, from.y + dy * patrolT};

    auto distTo = [](Vector2 a, Vector2 b)
    {
        return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    };

    Vector2 playCentre = {playBtn.x + playBtn.width / 2.f,
                          playBtn.y + playBtn.height / 2.f};
    Vector2 settingsCentre = waypoints[HOVER_WAYPOINT];

    float minDist = std::min(distTo(curPos, settingsCentre),
                             distTo(curPos, playCentre));

    float speed = (minDist < 150.f)
                      ? 60.f + (minDist / 150.f) * 120.f
                      : 180.f;

    patrolT += (speed * dt) / segLen;

    if (patrolT >= 1.f)
    {
        if (nextWP == HOVER_WAYPOINT)
        {
            isHovering = true;
            hoverTimer = 0.f;
            shipMenuPos = waypoints[HOVER_WAYPOINT];
        }
        else
        {
            patrolWaypoint = nextWP;
            patrolT = 0.f;
        }
        return;
    }

    shipMenuPos.x = from.x + dx * patrolT;
    shipMenuPos.y = from.y + dy * patrolT;
    shipMenuAngle = lerpAngle(shipMenuAngle, atan2f(dy, dx), 0.15f);
}

// ---------------- ASTEROID UPDATE ----------------

static void UpdateAsteroids(float dt)
{
    float t = GetTime();

    for (auto &a : menuAsteroids)
    {
        a.position.x += a.velocity.x * dt;
        a.position.y += a.velocity.y * dt;

        a.position.y += sinf(t + a.wobbleOffset) * a.wobbleStrength * dt;

        a.rotation += a.rotationSpeed * dt;

        if (a.position.x < -220.f ||
            a.position.x > VIRTUAL_WIDTH + 220.f ||
            a.position.y < -220.f ||
            a.position.y > VIRTUAL_HEIGHT + 220.f)
        {
            int side = GetRandomValue(0, 2);

            if (side == 0)
            {
                a.position = {
                    (float)(VIRTUAL_WIDTH + GetRandomValue(50, 250)),
                    (float)GetRandomValue(0, VIRTUAL_HEIGHT)};

                a.velocity = {
                    -(20.f + a.layer * 18.f),
                    8.f + (float)GetRandomValue(-10, 20)};
            }
            else if (side == 1)
            {
                a.position = {
                    (float)GetRandomValue(0, VIRTUAL_WIDTH),
                    (float)(VIRTUAL_HEIGHT + GetRandomValue(50, 250))};

                a.velocity = {
                    15.f + a.layer * 12.f,
                    -(25.f + a.layer * 10.f)};
            }
            else
            {
                a.position = {
                    -220.f,
                    (float)GetRandomValue(0, VIRTUAL_HEIGHT)};

                a.velocity = {
                    25.f + a.layer * 18.f,
                    6.f + (float)GetRandomValue(-12, 18)};
            }
        }
    }
}

// ---------------- PUBLIC FUNCTIONS ----------------

void InitMenu()
{
    shipTexture = LoadTexture("assets/Spaceship.png");
    SetTextureFilter(shipTexture, TEXTURE_FILTER_BILINEAR);

    settingsIcon = LoadTexture("assets/icons/settings.png");
    SetTextureFilter(settingsIcon, TEXTURE_FILTER_BILINEAR);

    asteroidTexture = LoadTexture("assets/ast.png");
    SetTextureFilter(asteroidTexture, TEXTURE_FILTER_BILINEAR);

    for (int i = 0; i < 250; i++)
    {
        menuStars.push_back({(float)GetRandomValue(0, VIRTUAL_WIDTH),
                             (float)GetRandomValue(0, VIRTUAL_HEIGHT)});
    }

    // ---------------- INIT ASTEROIDS ----------------

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

        a.rotationSpeed =
            (float)GetRandomValue(-18, 18);

        a.wobbleOffset =
            (float)GetRandomValue(0, 1000);

        a.wobbleStrength =
            (float)GetRandomValue(2, 8);

        menuAsteroids.push_back(a);
    }
}

void UpdateMenu(GameState &state, float scale, int offsetX, int offsetY)
{
    float dt = GetFrameTime();
    titlePulse += dt * 2.0f;
    starDrift += dt * 30.0f;

    UpdateMenuShip(dt);
    UpdateAsteroids(dt);

    Vector2 mouse = GetMousePosition();
    mouse.x = (mouse.x - offsetX) / scale;
    mouse.y = (mouse.y - offsetY) / scale;

    bool hoveringOnSettings = CheckCollisionPointRec(mouse, settingsBtn);
    if (hoveringOnSettings && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        showSettings = !showSettings;

    bool hovering = CheckCollisionPointRec(mouse, playBtn);

    if (showSettings && IsKeyPressed(KEY_ESCAPE))
        showSettings = false;

    if (!showSettings)
    {
        if ((hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
            IsKeyPressed(KEY_ENTER))
        {
            state = PLAYING;
        }
    }
}

void DrawMenu()
{
    DrawNebula();
    DrawStars();
    DrawAsteroids();
    DrawEngineTrail();
    DrawSpaceship(shipMenuPos, shipMenuAngle, 0.1f, 1.0f);
    DrawTitle();
    DrawPlayButton();
    DrawSettingsButton();

    if (showSettings)
        DrawSettingsPanel();
}

void UnloadMenu()
{
    UnloadTexture(shipTexture);
    UnloadTexture(settingsIcon);
    UnloadTexture(asteroidTexture);
}