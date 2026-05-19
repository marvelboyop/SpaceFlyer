#include "settings_button.h"

#include "../../core/constants.h"

#include "raylib.h"

static Rectangle settingsBtn = {
    VIRTUAL_WIDTH - 90,
    30,
    50,
    50};

static Texture2D settingsIcon;

void InitSettingsButton()
{
    settingsIcon =
        LoadTexture(
            "assets/icons/settings.png");

    SetTextureFilter(
        settingsIcon,
        TEXTURE_FILTER_BILINEAR);
}

void UpdateSettingsButton(
    bool &showSettings,
    float scale,
    int offsetX,
    int offsetY)
{
    Vector2 mouse =
        GetMousePosition();

    mouse.x =
        (mouse.x - offsetX) / scale;

    mouse.y =
        (mouse.y - offsetY) / scale;

    bool hoveringOnSettings =
        CheckCollisionPointRec(
            mouse,
            settingsBtn);

    if (hoveringOnSettings &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        showSettings = !showSettings;
    }
}

void DrawSettingsButton()
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

    bool hoveringOnSettings =
        CheckCollisionPointRec(
            mouse,
            settingsBtn);

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

    Vector2 iconPos = {
        settingsBtn.x + (settingsBtn.width - finalWidth) / 2,
        settingsBtn.y + (settingsBtn.height - finalHeight) / 2};

    DrawTextureEx(
        settingsIcon,
        iconPos,
        0.0f,
        iconScale,
        WHITE);
}