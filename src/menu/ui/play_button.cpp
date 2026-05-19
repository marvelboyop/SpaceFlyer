#include "play_button.h"

#include "../../core/constants.h"

static Rectangle playBtn = {
    500,
    510,
    280,
    90};

void InitPlayButton()
{
}

Rectangle GetPlayButtonBounds()
{
    return playBtn;
}

void UpdatePlayButton(
    GameState &state,
    float scale,
    int offsetX,
    int offsetY,
    bool showSettings)
{
    Vector2 mouse =
        GetMousePosition();

    mouse.x =
        (mouse.x - offsetX) / scale;

    mouse.y =
        (mouse.y - offsetY) / scale;

    bool hovering =
        CheckCollisionPointRec(mouse, playBtn);

    if (!showSettings)
    {
        if ((hovering &&
             IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
            IsKeyPressed(KEY_ENTER))
        {
            state = PLAYING;
        }
    }
}

void DrawPlayButton()
{
    Vector2 mouse =
        GetMousePosition();

    float scaleX =
        (float)GetRenderWidth() / VIRTUAL_WIDTH;

    float scaleY =
        (float)GetRenderHeight() / VIRTUAL_HEIGHT;

    float scale =
        (scaleX < scaleY) ? scaleX : scaleY;

    int drawW =
        (int)(VIRTUAL_WIDTH * scale);

    int drawH =
        (int)(VIRTUAL_HEIGHT * scale);

    int offsetX =
        (GetRenderWidth() - drawW) / 2;

    int offsetY =
        (GetRenderHeight() - drawH) / 2;

    mouse.x =
        (mouse.x - offsetX) / scale;

    mouse.y =
        (mouse.y - offsetY) / scale;

    bool hovering =
        CheckCollisionPointRec(mouse, playBtn);

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

    int textWidth =
        MeasureText(text, fontSize);

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