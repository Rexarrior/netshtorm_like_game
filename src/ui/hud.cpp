#include "hud.h"
#include "../core/config.h"

namespace ns {

void HUD::render(int energy, float game_speed, bool paused) const {
    // Top bar background
    DrawRectangle(0, 0, SCREEN_WIDTH, 40, Fade(BLACK, 0.7f));

    // Energy display
    std::string energy_text = "Storm Energy: " + std::to_string(energy);
    DrawText(energy_text.c_str(), 10, 10, 18, GOLD);

    // Speed display
    std::string speed_text = "Speed: ";
    int speed_int = static_cast<int>(game_speed);
    for (int i = 0; i < speed_int; ++i) speed_text += ">";
    if (paused) speed_text = "PAUSED";
    int speed_width = MeasureText(speed_text.c_str(), 16);
    DrawText(speed_text.c_str(), SCREEN_WIDTH / 2 - speed_width / 2, 12, 16, WHITE);

    // FPS
    std::string fps_text = "FPS: " + std::to_string(GetFPS());
    int fps_width = MeasureText(fps_text.c_str(), 14);
    DrawText(fps_text.c_str(), SCREEN_WIDTH - fps_width - 10, 12, 14, GRAY);
}

} // namespace ns
