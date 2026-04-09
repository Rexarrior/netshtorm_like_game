#include "minimap.h"
#include "../camera/game_camera.h"
#include "../core/config.h"

namespace ns {

Minimap::Minimap(int x, int y, int size) : x_(x), y_(y), size_(size) {}

void Minimap::render(const GameMap& map, GameCamera& game_cam) {
    (void)game_cam;

    DrawRectangle(x_, y_, size_, size_, Fade(BLACK, 0.6f));
    DrawRectangleLines(x_, y_, size_, size_, DARKGRAY);

    float scale = static_cast<float>(size_) / (MAP_GRID_SIZE * 2.0f);
    float offset_x = x_ + size_ / 2.0f;
    float offset_y = y_ + size_ / 2.0f;

    for (const auto& island : map.islands()) {
        Color col;
        switch (island.type) {
            case IslandType::Home: col = GREEN; break;
            case IslandType::Enemy: col = RED; break;
            case IslandType::Resource: col = SKYBLUE; break;
            case IslandType::Strategic: col = YELLOW; break;
        }
        float mx = offset_x + (island.center.x - MAP_GRID_SIZE / 2) * scale;
        float my = offset_y + (island.center.y - MAP_GRID_SIZE / 2) * scale;
        DrawCircle(static_cast<int>(mx), static_cast<int>(my), 4, col);
    }

    for (const auto& bridge : map.bridges()) {
        if (!bridge.is_alive()) continue;
        Color col = bridge.owner_id == 0 ? GREEN : RED;
        for (const auto& tile : bridge.occupied_tiles) {
            float mx = offset_x + (tile.gx - MAP_GRID_SIZE / 2) * scale;
            float my = offset_y + (tile.gy - MAP_GRID_SIZE / 2) * scale;
            DrawPixel(static_cast<int>(mx), static_cast<int>(my), col);
        }
    }
}

} // namespace ns