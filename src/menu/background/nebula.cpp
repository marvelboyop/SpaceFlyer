#include "nebula.h"
#include "raylib.h"
void DrawNebula()
{
    DrawCircleV({150.f, 120.f}, 130.f, Fade(Color{58, 22, 110, 255}, 0.35f));
    DrawCircleV({670.f, 480.f}, 160.f, Fade(Color{92, 36, 140, 255}, 0.28f));
    DrawCircleV({400.f, 300.f}, 220.f, Fade(Color{25, 18, 70, 255}, 0.40f));
    DrawCircleV({750.f, 150.f}, 100.f, Fade(Color{140, 40, 120, 255}, 0.22f));
    DrawCircleV({80.f, 400.f}, 80.f, Fade(Color{30, 90, 120, 255}, 0.18f));
    DrawCircleV({980.f, 420.f}, 220.f, Fade(Color{58, 22, 110, 255}, 0.35f));
}