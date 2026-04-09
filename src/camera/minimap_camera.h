#pragma once

#include <raylib.h>

namespace ns {

class MinimapCamera {
public:
    MinimapCamera(int map_size = 32);
    void update(class GameCamera& game_cam);
    void render();
    void set_position(int x, int y);
    Rectangle viewport() const;

private:
    int map_size_;
    int minimap_x_ = 0;
    int minimap_y_ = 0;
    int minimap_size_ = 160;
};

} // namespace ns