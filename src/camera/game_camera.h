#pragma once

#include <raylib.h>
#include "../core/types.h"

namespace ns {

class GameCamera {
public:
    GameCamera();
    void update(float dt, class InputHandler& input);
    Camera2D camera() const;
    void follow(Position grid_pos);
    Vector2 grid_to_screen(int gx, int gy) const;
    Position screen_to_grid(Vector2 screen_pos) const;
    void set_zoom(float z);
    float get_zoom() const;

private:
    Camera2D camera_{};
    float zoom_ = 1.0f;
    float pan_speed_ = 300.0f;
};

} // namespace ns
