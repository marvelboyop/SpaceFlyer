#include "stars.h"
#include "raylib.h"
#include "../../core/constants.h"
#include <vector>
#include <cmath>
extern float starDrift;
static std::vector<Vector2> menuStars;
void InitStars()
{
    for (int i = 0; i < 250; i++)
    {
        menuStars.push_back({(float)GetRandomValue(0, VIRTUAL_WIDTH),
                             (float)GetRandomValue(0, VIRTUAL_HEIGHT)});
    }
}
void DrawStars()
{
    for (auto &st : menuStars)
    {
        float sx = fmodf(
            st.x - starDrift * 0.25f + VIRTUAL_WIDTH,
            (float)VIRTUAL_WIDTH);
        DrawPixelV(Vector2{sx, st.y}, Color{180, 200, 255, 200});
    }
}