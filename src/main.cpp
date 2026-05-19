#include "raylib.h"
#include "core/constants.h"
#include "core/gamestate.h"
#include "menu/menu.h"
#include <algorithm>
int main()
{
    GameState state = MENU;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "SpaceFlyer");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    RenderTexture2D target =
        LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    InitMenu();
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        shipPulse += dt;
        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }
        if (IsKeyPressed(KEY_F))
        {
            UnloadRenderTexture(target);
            ToggleFullscreen();
            target = LoadRenderTexture(
                VIRTUAL_WIDTH,
                VIRTUAL_HEIGHT);
        }
        int screenW = GetRenderWidth();
        int screenH = GetRenderHeight();
        float scaleX =
            (float)screenW / (float)VIRTUAL_WIDTH;
        float scaleY =
            (float)screenH / (float)VIRTUAL_HEIGHT;
        float scale =
            std::min(scaleX, scaleY);
        int drawW =
            (int)(VIRTUAL_WIDTH * scale);
        int drawH =
            (int)(VIRTUAL_HEIGHT * scale);
        int offsetX =
            (screenW - drawW) / 2;
        int offsetY =
            (screenH - drawH) / 2;
        BeginTextureMode(target);
        ClearBackground(BLACK);
        switch (state)
        {
        case MENU:
            UpdateMenu(state, scale, offsetX, offsetY);
            DrawMenu();
            break;
        case PLAYING:
            ClearBackground(BLACK);
            if (IsKeyPressed(KEY_R))
            {
                state = MENU;
            }
            break;
        default:
            break;
        }
        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        Rectangle source = {
            0.0f,
            0.0f,
            (float)target.texture.width, -(float)target.texture.height};
        Rectangle dest = {
            (float)offsetX,
            (float)offsetY,
            (float)drawW,
            (float)drawH};
        DrawTexturePro(
            target.texture,
            source,
            dest,
            {0, 0},
            0.0f,
            WHITE);
        EndDrawing();
    }
    UnloadMenu();
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}