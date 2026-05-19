#include "title.h"

#include "raylib.h"

#include "../../core/constants.h"

#include <cmath>

extern float titlePulse;

void DrawTitle()
{
    float pulse =
        0.85f + 0.15f * sinf(titlePulse);

    Color titleCol = {
        (unsigned char)(70.f * pulse),
        (unsigned char)(170.f * pulse),
        255,
        255};

    DrawText(
        "Space Flyer",
        VIRTUAL_WIDTH / 2 - 272,
        VIRTUAL_HEIGHT / 2 - 173,
        90,
        Fade(SKYBLUE, 0.30f));

    DrawText(
        "Space Flyer",
        VIRTUAL_WIDTH / 2 - 270,
        VIRTUAL_HEIGHT / 2 - 170,
        90,
        titleCol);

    DrawText(
        "Precision is Survival.",
        VIRTUAL_WIDTH / 2 - 120,
        VIRTUAL_HEIGHT / 2 - 68,
        24,
        Color{112, 128, 144, 200});

    DrawText(
        "[Arrow Keys] steer     [F] Fullscreen",
        VIRTUAL_WIDTH / 2 - 160,
        VIRTUAL_HEIGHT / 2 + 20,
        18,
        Color{211, 211, 211, 155});

    float enterAlpha =
        0.55f + 0.45f * sinf(titlePulse * 2.0f);

    DrawText(
        "PRESS ENTER TO START",
        VIRTUAL_WIDTH / 2 - 133,
        VIRTUAL_HEIGHT / 2 - 20,
        20,
        Fade(WHITE, enterAlpha));
}