#pragma once

#ifdef USE_RAYLIB_STUB
#include "raylib_stub.h"
#else
#include <raylib.h>
#endif
#include "../core/types.h"
#include "../core/config.h"

namespace ns {

class IsometricCamera2D {
public:
    IsometricCamera2D(Vector2 offset, float initial_zoom = 1.0f);
    
    Vector2 grid_to_screen(int gx, int gy) const;
    Vector2 grid_to_screen(float gx, float gy) const;
    Position screen_to_grid(Vector2 screen) const;
    
    void begin() const;
    void end() const;
    
    void follow(int gx, int gy);
    void pan(int dx, int dy);
    void pan_to(int gx, int gy);
    void update(float dt);
    void snap_to_target();  // Instantly move visual to target
    
    void zoom_at(float factor, Vector2 screen_pos);
    void set_zoom(float z);
    float get_zoom() const;
    
    int target_grid_x() const;
    int target_grid_y() const;
    int visual_grid_x() const;
    int visual_grid_y() const;
    float get_follow_speed() const;
    void set_follow_speed(float s);
    
    Vector2 get_offset() const { return offset_; }

private:
    Vector2 offset_;
    float zoom_;
    int grid_x_, grid_y_;
    int visual_x_, visual_y_;
    float follow_speed_;
};

} // namespace ns