#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <vector>

enum GameState
{
    MENU,
    PLAYING,
    GAME_OVER,
    WIN
};
struct Player
{
    Vector2 position, direction;
    float speed;
};
struct Particle
{
    Vector2 position, velocity;
    float life;
};
struct Segment
{
    Vector2 start, end;
};

static const int VIRTUAL_WIDTH = 1280;
static const int VIRTUAL_HEIGHT = 720;
float pipeWidth = 60.0f;

int main()
{
    GameState state = MENU;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "SpaceFlyer");
    SetTargetFPS(60);

    // Esc key handling
    SetExitKey(KEY_NULL);

    // This is the off-screen buffer where the game is actually drawn.
    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

    std::vector<Vector2> menuStars;
    for (int i = 0; i < 250; i++)
        menuStars.push_back({(float)GetRandomValue(0, VIRTUAL_WIDTH), (float)GetRandomValue(0, VIRTUAL_HEIGHT)});

    Rectangle playBtn = {500, 510, 280, 90};

    bool showSettings = false;

    Rectangle settingsBtn = {
        VIRTUAL_WIDTH - 90,
        30,
        50,
        50};

    Texture2D settingsIcon = LoadTexture("assets/icons/settings.png");

    SetTextureFilter(
        settingsIcon,
        TEXTURE_FILTER_BILINEAR);

    float titlePulse = 0;
    float starDrift = 0;

    while (!WindowShouldClose())
    {
        titlePulse += GetFrameTime() * 2.0f;
        starDrift += GetFrameTime() * 30.0f;

        if (!showSettings && IsKeyPressed(KEY_ESCAPE))
        {
            CloseWindow();
        }

        if (IsKeyPressed(KEY_F))
        {
            UnloadRenderTexture(target);

            ToggleFullscreen();

            target = LoadRenderTexture(
                VIRTUAL_WIDTH,
                VIRTUAL_HEIGHT);
        }

        // Actual window size on the current display.
        // GetRenderWidth/Height are useful on HiDPI displays.
        int screenW = GetRenderWidth();
        int screenH = GetRenderHeight();

        // Keep aspect ratio and fit inside the screen.
        float scaleX = (float)screenW / (float)VIRTUAL_WIDTH;
        float scaleY = (float)screenH / (float)VIRTUAL_HEIGHT;
        float scale = std::min(scaleX, scaleY);

        int drawW = (int)(VIRTUAL_WIDTH * scale);
        int drawH = (int)(VIRTUAL_HEIGHT * scale);

        int offsetX = (screenW - drawW) / 2;
        int offsetY = (screenH - drawH) / 2;

        // Draw the whole game into the render texture.
        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (state == MENU)
        {
            if (showSettings && IsKeyPressed(KEY_ESCAPE))
            {
                showSettings = false;
            }

            // Nebula background: soft glowing clouds
            DrawCircleV({150.f, 120.f}, 130.f, Fade(Color{58, 22, 110, 255}, 0.35f));

            DrawCircleV({670.f, 480.f}, 160.f, Fade(Color{92, 36, 140, 255}, 0.28f));

            DrawCircleV({400.f, 300.f}, 220.f, Fade(Color{25, 18, 70, 255}, 0.40f));

            DrawCircleV({750.f, 150.f}, 100.f, Fade(Color{140, 40, 120, 255}, 0.22f));

            DrawCircleV({80.f, 400.f}, 80.f, Fade(Color{30, 90, 120, 255}, 0.18f));

            DrawCircleV({980.f, 420.f}, 220.f, Fade(Color{58, 22, 110, 255}, 0.35f));

            // Drifting parallax stars
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

            // Shadow pass (glow under title)
            float pulse = 0.85f + 0.15f * sinf(titlePulse);

            Color titleCol = {
                (unsigned char)(70.f * pulse),
                (unsigned char)(170.f * pulse),
                255,
                255};

            DrawText("Space Flyer", VIRTUAL_WIDTH / 2 - 272, VIRTUAL_HEIGHT / 2 - 173, 90, Fade(SKYBLUE, 0.30f));
            // Main title
            DrawText("Space Flyer", VIRTUAL_WIDTH / 2 - 270, VIRTUAL_HEIGHT / 2 - 170, 90, titleCol);

            // Tagline
            DrawText("Precision is Survival.",
                     VIRTUAL_WIDTH / 2 - 120, VIRTUAL_HEIGHT / 2 - 68, 24, Color{112, 128, 144, 200});

            // Controls hint (subtle, always visible)
            DrawText("[Arrow Keys] steer     [F] Fullscreen",
                     VIRTUAL_WIDTH / 2 - 160, VIRTUAL_HEIGHT / 2 + 20, 18, Color{211, 211, 211, 155});

            float enterAlpha =
                0.55f + 0.45f * sinf(titlePulse * 2.0f);

            // Blinking "PRESS ENTER" prompt
            DrawText("PRESS ENTER TO START",
                     VIRTUAL_WIDTH / 2 - 133, VIRTUAL_HEIGHT / 2 - 20, 20, Fade(WHITE, enterAlpha));

            //-----------------------PLAY BUTTON UI---------------------------------------------
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

            Color btn = hovering
                            ? Color{35, 70, 140, 255} // brighter, slightly more saturated hover base
                            : Color{22, 38, 74, 255};

            Color glow = hovering
                             ? Fade(Color{80, 180, 255, 255}, 0.35f) // stronger glow on hover
                             : Fade(Color{80, 180, 255, 255}, 0.18f);

            // Outer glow
            DrawRectangleRounded(
                {playBtn.x - 4,
                 playBtn.y - 4,
                 playBtn.width + 8,
                 playBtn.height + 8},
                0.30f,
                8,
                glow // CHANGED: uses dynamic glow instead of fixed fade
            );

            // Main button
            DrawRectangleRounded(
                playBtn,
                0.28f,
                8,
                btn);

            // Top shine
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

            //  hover text glow improved for nebula contrast

            Color textGlow = hovering
                                 ? Fade(SKYBLUE, 0.55f)
                                 : Fade(SKYBLUE, 0.35f);

            // Glow shadow
            DrawText(
                text,
                textX + 2,
                textY + 2,
                fontSize,
                textGlow);

            // Main text
            DrawText(
                text,
                textX,
                textY,
                fontSize,
                Color{220, 240, 255, 255});

            // ---------------- SETTINGS BUTTON ----------------

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

            if (showSettings)
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

            if (!showSettings)
            {
                if ((hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
                    IsKeyPressed(KEY_ENTER))
                {
                    state = PLAYING;
                }
            }
        }

        if (state == PLAYING)
        {
            ClearBackground(BLACK);
            if (IsKeyPressed(KEY_R))
            {
                state = MENU;
            }
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        Rectangle source = {
            0.0f,
            0.0f,
            (float)target.texture.width,
            -(float)target.texture.height};

        Rectangle dest = {
            (float)offsetX,
            (float)offsetY,
            (float)drawW,
            (float)drawH};
        DrawTexturePro(target.texture, source, dest, {0, 0}, 0.0f, WHITE);

        EndDrawing();
    }

    UnloadTexture(settingsIcon);
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}