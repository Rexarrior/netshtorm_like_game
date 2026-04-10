#include "game_camera.h"
#include "../input/input_handler.h"
#include "../core/config.h"

namespace ns {

GameCamera::GameCamera() {
    impl_ = std::make_unique<IsometricCamera2D>(
        Vector2{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, 1.0f
    );
}

void GameCamera::update(float dt, InputHandler& input) {
    int pan_dx = 0, pan_dy = 0;
    if (input.is_action_pressed(InputAction::PanUp)) pan_dy -= 1;
    if (input.is_action_pressed(InputAction::PanDown)) pan_dy += 1;
    if (input.is_action_pressed(InputAction::PanLeft)) pan_dx -= 1;
    if (input.is_action_pressed(InputAction::PanRight)) pan_dx += 1;
    
    if (pan_dx != 0 || pan_dy != 0) {
        impl_->pan(pan_dx, pan_dy);
    }
    
    float wheel = input.get_mouse_wheel_move();
    if (wheel != 0) {
        impl_->zoom_at(wheel > 0 ? 1.1f : 0.9f, input.get_mouse_position());
    }
    
    impl_->update(dt);
}

Camera2D GameCamera::camera() const { 
    Camera2D cam = {};
    cam.offset = impl_->get_offset();
    cam.target = {0, 0};
    cam.rotation = 0;
    cam.zoom = impl_->get_zoom();
    return cam;
}

void GameCamera::follow(Position grid_pos) {
    impl_->follow(grid_pos.x, grid_pos.y);
}

Vector2 GameCamera::grid_to_screen(int gx, int gy) const {
    return impl_->grid_to_screen(gx, gy);
}

Position GameCamera::screen_to_grid(Vector2 screen_pos) const {
    return impl_->screen_to_grid(screen_pos);
}

void GameCamera::set_zoom(float z) {
    impl_->set_zoom(z);
}

float GameCamera::get_zoom() const {
    return impl_->get_zoom();
}

void GameCamera::snap_to_target() {
    impl_->snap_to_target();
}

} // namespace ns