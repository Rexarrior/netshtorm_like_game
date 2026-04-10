#pragma once

#include <raylib.h>
#include "../core/types.h"
#include "isometric_camera.h"
#include <memory>

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
    void snap_to_target();  // Instantly move visual to target
    operator IsometricCamera2D&() { return *impl_; }
    operator const IsometricCamera2D&() const { return *impl_; }

private:
    std::unique_ptr<IsometricCamera2D> impl_;
};

} // namespace ns