#include "settings_panel.h"

#include "raylib.h"

void DrawSettingsPanel()
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