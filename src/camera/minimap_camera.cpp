#include "minimap_camera.h"
#include "game_camera.h"

namespace ns {

MinimapCamera::MinimapCamera(int map_size) : map_size_(map_size) {}

void MinimapCamera::update(GameCamera& game_cam) {
    (void)game_cam;
}

void MinimapCamera::render() {
    DrawRectangle(minimap_x_, minimap_y_, minimap_size_, minimap_size_, DARKGRAY);
    DrawRectangleLines(minimap_x_, minimap_y_, minimap_size_, minimap_size_, WHITE);
}

void MinimapCamera::set_position(int x, int y) {
    minimap_x_ = x;
    minimap_y_ = y;
}

Rectangle MinimapCamera::viewport() const {
    return {static_cast<float>(minimap_x_), static_cast<float>(minimap_y_),
            static_cast<float>(minimap_size_), static_cast<float>(minimap_size_)};
}

} // namespace ns