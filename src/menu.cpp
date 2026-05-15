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
static const int HOVER_WAYPOINT = 1; // index of settings btn in waypoints

// Engine trail
struct TrailPoint
{
    Vector2 pos;
    float alpha;
};
static std::vector<TrailPoint> engineTrail;

// Waypoints: bottom-left → settings → bottom-right → top-left
static const Vector2 waypoints[] = {
    {150.f, 620.f},                        // 0  bottom-left
    {(float)VIRTUAL_WIDTH - 65.f, 55.f},   // 1  settings btn centre
    {(float)VIRTUAL_WIDTH - 150.f, 620.f}, // 2  bottom-right
    {150.f, 100.f},                        // 3  top-left
};
static const int WAYPOINT_COUNT = 4;

// Animation timers
float menuT = 0.f;      // figure-8 path parameter
float titlePulse = 0.f; // title glow oscillation
float shipPulse = 0.f;  // engine flame oscillation (always ticks)
float starDrift = 0.f;  // menu star parallax offset

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
        float sx =
            fmodf(
                st.x - starDrift * 0.25f +
                    VIRTUAL_WIDTH,
                (float)VIRTUAL_WIDTH);

        DrawPixelV(
            Vector2{sx, st.y},
            Color{180, 200, 255, 200});
    }
}

static void DrawSpaceship(Vector2 pos, float angle, float scale, float alpha)
{
    float w = shipTexture.width * scale;
    float h = shipTexture.height * scale;
    float s = scale * 8.f; // ← ADD THIS

    Rectangle source = {0, 0, (float)shipTexture.width, (float)shipTexture.height};
    Rectangle dest = {pos.x, pos.y, w, h};
    Vector2 origin = {w / 2.f, h / 2.f};

    float angleDeg = angle * RAD2DEG + 90.f;

    // Engine position (behind the ship)
    Vector2 back = {
        pos.x - cosf(angle) * (h / 2.f),
        pos.y - sinf(angle) * (h / 2.f)};

    // Flame pulse
    float fp = 0.55f + 0.45f * sinf(shipPulse * 12.f);

    Vector2 flameTip = {
        back.x - cosf(angle) * 48.f * fp,
        back.y - sinf(angle) * 48.f * fp};

    Vector2 flameL = {back.x + cosf(angle + PI / 2.f) * 10.f,
                      back.y + sinf(angle + PI / 2.f) * 10.f};

    Vector2 flameR = {back.x + cosf(angle - PI / 2.f) * 10.f,
                      back.y + sinf(angle - PI / 2.f) * 10.f};

    DrawTriangle(flameTip, flameL, flameR, Fade(Color{255, 60, 0, 255}, alpha * 0.90f));
    DrawTriangle(flameTip, flameL, flameR, Fade(YELLOW, alpha * 0.50f));

    // Oval engine glow — stretched along backward direction
    for (int i = 0; i < 9; i++)
    {
        float t = (float)i / 8.f; // 0 = at nozzle, 1 = tail end

        Vector2 p = {
            back.x + cosf(angle + PI) * 10.f * t * s,
            back.y + sinf(angle + PI) * 10.f * t * s};

        float radius = (10.f - 5.f * t) * scale * 8.f; // wide at nozzle, thin at tail

        DrawCircleV(p, radius, Fade(ORANGE, alpha * 0.85f * (1.f - t * 0.4f)));
        DrawCircleV(p, radius * 0.9f, Fade(YELLOW, alpha * (1.f - t * 0.5f)));
    }

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

    float enterAlpha =
        0.55f + 0.45f * sinf(titlePulse * 2.0f);

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

    Color btn =
        hovering
            ? Color{35, 70, 140, 255}
            : Color{22, 38, 74, 255};

    Color glow =
        hovering
            ? Fade(Color{80, 180, 255, 255}, 0.35f)
            : Fade(Color{80, 180, 255, 255}, 0.18f);

    DrawRectangleRounded(
        {playBtn.x - 4,
         playBtn.y - 4,
         playBtn.width + 8,
         playBtn.height + 8},
        0.30f,
        8,
        glow);

    DrawRectangleRounded(
        playBtn,
        0.28f,
        8,
        btn);

    DrawRectangleRounded(
        {playBtn.x,
         playBtn.y,
         playBtn.width,
         playBtn.height / 2},
        0.28f,
        8,
        Fade(WHITE, 0.05f));

    const char *text = "PLAY";

    int fontSize = 42;

    int textWidth = MeasureText(text, fontSize);

    int textX =
        playBtn.x +
        (playBtn.width - textWidth) / 2;

    int textY =
        playBtn.y +
        (playBtn.height - fontSize) / 2;

    Color textGlow =
        hovering
            ? Fade(SKYBLUE, 0.55f)
            : Fade(SKYBLUE, 0.35f);

    DrawText(
        text,
        textX + 2,
        textY + 2,
        fontSize,
        textGlow);

    DrawText(
        text,
        textX,
        textY,
        fontSize,
        Color{220, 240, 255, 255});
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

    bool hoveringOnSettings =
        CheckCollisionPointRec(mouse, settingsBtn);

    Color settingsColor =
        hoveringOnSettings
            ? Color{50, 90, 170, 255}
            : Color{25, 45, 85, 255};

    DrawRectangleRounded(
        settingsBtn,
        0.50f,
        8,
        settingsColor);

    float desiredSize = 40.0f;

    float iconScale =
        desiredSize / settingsIcon.width;

    float finalWidth =
        settingsIcon.width * iconScale;

    float finalHeight =
        settingsIcon.height * iconScale;

    Vector2 iconPos =
        {
            settingsBtn.x + (settingsBtn.width - finalWidth) / 2,
            settingsBtn.y + (settingsBtn.height - finalHeight) / 2};

    DrawTextureEx(
        settingsIcon,
        iconPos,
        0.0f,
        iconScale,
        WHITE);
}

static void DrawSettingsPanel()
{
    DrawRectangleRounded(
        {340, 120, 600, 420},
        0.04f,
        10,
        BLACK);

    DrawRectangleRoundedLinesEx(
        {340, 120, 600, 420},
        0.04f,
        10,
        2,
        Fade(SKYBLUE, 0.5f));

    DrawText(
        "SETTINGS",
        535,
        150,
        40,
        SKYBLUE);

    DrawText(
        "Controls",
        420,
        240,
        28,
        WHITE);

    DrawText(
        "Arrow Keys -> Move",
        420,
        290,
        22,
        LIGHTGRAY);

    DrawText(
        "F -> Fullscreen",
        420,
        330,
        22,
        LIGHTGRAY);

    DrawText(
        "ENTER -> Start Game",
        420,
        370,
        22,
        LIGHTGRAY);

    DrawText(
        "ESC -> Close Settings",
        420,
        410,
        22,
        LIGHTGRAY);
}

//--------------Engine Trail-------------------
static void DrawEngineTrail()
{
    for (auto &p : menuParticles)
    {
        DrawCircleV(p.position, 3.f, Fade(ORANGE, p.life));
        DrawCircleV(p.position, 5.5f, Fade(YELLOW, p.life * 0.5f));
    }
}

// ---------------- PUBLIC FUNCTIONS ----------------

void InitMenu()
{
    shipTexture = LoadTexture("assets/Spaceship.png");
    SetTextureFilter(shipTexture, TEXTURE_FILTER_BILINEAR);

    settingsIcon = LoadTexture("assets/icons/settings.png");
    SetTextureFilter(
        settingsIcon,
        TEXTURE_FILTER_BILINEAR);

    for (int i = 0; i < 250; i++)
    {
        menuStars.push_back({(float)GetRandomValue(0, VIRTUAL_WIDTH),
                             (float)GetRandomValue(0, VIRTUAL_HEIGHT)});
    }
}

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
    // --- Fade trail points ---
    for (auto &tp : engineTrail)
        tp.alpha -= dt * 2.0f;

    engineTrail.erase(
        std::remove_if(engineTrail.begin(), engineTrail.end(),
                       [](const TrailPoint &tp)
                       { return tp.alpha <= 0.f; }),
        engineTrail.end());

    // --- Hovering around settings btn ---
    if (isHovering)
    {
        hoverTimer += dt;
        hoverOrbitAngle += dt * 2.5f; // orbit speed

        Vector2 centre = waypoints[HOVER_WAYPOINT];
        shipMenuPos.x = centre.x + cosf(hoverOrbitAngle) * HOVER_RADIUS;
        shipMenuPos.y = centre.y + sinf(hoverOrbitAngle) * HOVER_RADIUS;
        shipMenuAngle = hoverOrbitAngle + PI / 2.f; // face tangent

        engineTrail.push_back({shipMenuPos, 1.0f});

        if (hoverTimer >= HOVER_DURATION)
        {
            isHovering = false;
            hoverTimer = 0.f;
            patrolWaypoint = HOVER_WAYPOINT;
            patrolT = 0.f;
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

    // Current position on segment
    Vector2 curPos = {from.x + dx * patrolT, from.y + dy * patrolT};

    // Speed variation — slow near settings btn and play btn
    auto distTo = [&](Vector2 a, Vector2 b)
    {
        return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    };

    Vector2 playCentre = {playBtn.x + playBtn.width / 2.f,
                          playBtn.y + playBtn.height / 2.f};
    Vector2 settingsCentre = waypoints[HOVER_WAYPOINT];

    float minDist = std::min(distTo(curPos, settingsCentre),
                             distTo(curPos, playCentre));

    float speed = (minDist < 150.f)
                      ? 60.f + (minDist / 150.f) * 120.f // 60 near UI, 180 far
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
    shipMenuAngle = atan2f(dy, dx);

    engineTrail.push_back({shipMenuPos, 1.0f});
}

void UpdateMenu(GameState &state, float scale, int offsetX, int offsetY)
{
    float dt = GetFrameTime();
    menuT += dt * 0.6f;
    titlePulse += dt * 2.0f;
    starDrift += dt * 30.0f;

    UpdateMenuShip(dt);

    Vector2 mouse = GetMousePosition();

    mouse.x = (mouse.x - offsetX) / scale;
    mouse.y = (mouse.y - offsetY) / scale;

    bool hoveringOnSettings =
        CheckCollisionPointRec(mouse, settingsBtn);

    if (hoveringOnSettings && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        showSettings = !showSettings;
    }

    bool hovering = CheckCollisionPointRec(mouse, playBtn);

    if (showSettings && IsKeyPressed(KEY_ESCAPE))
    {
        showSettings = false;
    }

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

    // --- draw everything ---
    DrawNebula();

    DrawStars();

    DrawEngineTrail();
    DrawSpaceship(shipMenuPos, shipMenuAngle, 0.1f, 1.0f); // uses stored state

    DrawTitle();

    DrawPlayButton();

    DrawSettingsButton();

    if (showSettings)
    {
        DrawSettingsPanel();
    }
}

void UnloadMenu()
{
    UnloadTexture(settingsIcon);
    UnloadTexture(settingsIcon);
}