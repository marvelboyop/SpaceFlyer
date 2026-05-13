#include "menu.h"

#include <cmath>
#include <vector>

static const int VIRTUAL_WIDTH = 1280;
static const int VIRTUAL_HEIGHT = 720;

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

static float titlePulse = 0.0f;
static float starDrift = 0.0f;

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

// ---------------- PUBLIC FUNCTIONS ----------------

void InitMenu()
{
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

void UpdateMenu(GameState &state, float scale, int offsetX, int offsetY)
{
    titlePulse += GetFrameTime() * 2.0f;
    starDrift += GetFrameTime() * 30.0f;

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
    DrawNebula();

    DrawStars();

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
}