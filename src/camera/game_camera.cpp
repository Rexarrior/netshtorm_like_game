#include "game_camera.h"
#include "../input/input_handler.h"
#include "../core/config.h"

namespace ns {

GameCamera::GameCamera() {
    camera_.target = {0, 0};
    camera_.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera_.rotation = 0.0f;
    camera_.zoom = 1.0f;
}

void GameCamera::update(float dt, InputHandler& input) {
    Vector2 pan = {0, 0};
    if (input.is_action_pressed(InputAction::PanUp)) pan.y -= pan_speed_ * dt;
    if (input.is_action_pressed(InputAction::PanDown)) pan.y += pan_speed_ * dt;
    if (input.is_action_pressed(InputAction::PanLeft)) pan.x -= pan_speed_ * dt;
    if (input.is_action_pressed(InputAction::PanRight)) pan.x += pan_speed_ * dt;
    camera_.target.x += pan.x;
    camera_.target.y += pan.y;

    float wheel = input.get_mouse_wheel_move();
    if (wheel != 0) {
        zoom_ += wheel * CAMERA_ZOOM_SPEED;
        if (zoom_ < CAMERA_MIN_ZOOM) zoom_ = CAMERA_MIN_ZOOM;
        if (zoom_ > CAMERA_MAX_ZOOM) zoom_ = CAMERA_MAX_ZOOM;

        // Zoom toward mouse
        Vector2 mouse = input.get_mouse_position();
        Vector2 world_before = GetScreenToWorld2D(mouse, camera_);
        camera_.zoom = zoom_;
        Vector2 world_after = GetScreenToWorld2D(mouse, camera_);
        camera_.target.x += (world_after.x - world_before.x);
        camera_.target.y += (world_after.y - world_before.y);
    } else {
        camera_.zoom = zoom_;
    }
}

Camera2D GameCamera::camera() const { return camera_; }

void GameCamera::follow(Position grid_pos) {
    Vector2 screen = grid_to_screen(grid_pos.x, grid_pos.y);
    camera_.target = screen;
}

Vector2 GameCamera::grid_to_screen(int gx, int gy) const {
    return {
        static_cast<float>((gx - gy) * (TILE_WIDTH / 2)),
        static_cast<float>((gx + gy) * (TILE_HEIGHT / 2))
    };
}

Position GameCamera::screen_to_grid(Vector2 screen_pos) const {
    Vector2 world = GetScreenToWorld2D(screen_pos, camera_);
    // Inverse isometric transform
    float gx = (world.x / (TILE_WIDTH / 2.0f) + world.y / (TILE_HEIGHT / 2.0f)) / 2.0f;
    float gy = (world.y / (TILE_HEIGHT / 2.0f) - world.x / (TILE_WIDTH / 2.0f)) / 2.0f;
    return {static_cast<int>(gx), static_cast<int>(gy)};
}

void GameCamera::set_zoom(float z) {
    zoom_ = z;
    camera_.zoom = z;
}

float GameCamera::get_zoom() const { return zoom_; }

} // namespace ns
