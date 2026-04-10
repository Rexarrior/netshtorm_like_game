#include "isometric_camera.h"
#include "../core/config.h"

namespace ns {

IsometricCamera2D::IsometricCamera2D(Vector2 offset, float initial_zoom)
    : offset_(offset)
    , zoom_(initial_zoom)
    , target_zoom_(initial_zoom)
    , zoom_speed_(5.0f)
    , grid_x_(0), grid_y_(0)
    , visual_x_(0), visual_y_(0)
    , follow_speed_(8.0f)
{
}

Vector2 IsometricCamera2D::grid_to_screen(int gx, int gy) const {
    float iso_x = (gx - gy) * (TILE_WIDTH / 2.0f);
    float iso_y = (gx + gy) * (TILE_HEIGHT / 2.0f);
    
    float cam_iso_x = (visual_x_ - visual_y_) * (TILE_WIDTH / 2.0f);
    float cam_iso_y = (visual_x_ + visual_y_) * (TILE_HEIGHT / 2.0f);
    
    float rel_x = iso_x - cam_iso_x;
    float rel_y = iso_y - cam_iso_y;
    
    return Vector2{
        offset_.x + rel_x * zoom_,
        offset_.y + rel_y * zoom_
    };
}

Vector2 IsometricCamera2D::grid_to_screen(float gx, float gy) const {
    float iso_x = (gx - gy) * (TILE_WIDTH / 2.0f);
    float iso_y = (gx + gy) * (TILE_HEIGHT / 2.0f);
    
    float cam_iso_x = (visual_x_ - visual_y_) * (TILE_WIDTH / 2.0f);
    float cam_iso_y = (visual_x_ + visual_y_) * (TILE_HEIGHT / 2.0f);
    
    float rel_x = iso_x - cam_iso_x;
    float rel_y = iso_y - cam_iso_y;
    
    return Vector2{
        offset_.x + rel_x * zoom_,
        offset_.y + rel_y * zoom_
    };
}

Position IsometricCamera2D::screen_to_grid(Vector2 screen) const {
    float rel_x = (screen.x - offset_.x) / zoom_;
    float rel_y = (screen.y - offset_.y) / zoom_;
    
    float cam_iso_x = (visual_x_ - visual_y_) * (TILE_WIDTH / 2.0f);
    float cam_iso_y = (visual_x_ + visual_y_) * (TILE_HEIGHT / 2.0f);
    
    float iso_x = rel_x + cam_iso_x;
    float iso_y = rel_y + cam_iso_y;
    
    float gx = (iso_x / (TILE_WIDTH / 2.0f) + iso_y / (TILE_HEIGHT / 2.0f)) / 2.0f;
    float gy = (iso_y / (TILE_HEIGHT / 2.0f) - iso_x / (TILE_WIDTH / 2.0f)) / 2.0f;
    
    return {static_cast<int>(gx), static_cast<int>(gy)};
}

void IsometricCamera2D::begin() const {
    Camera2D cam = {};
    cam.offset = offset_;
    cam.target = {0, 0};
    cam.rotation = 0;
    cam.zoom = zoom_;
    BeginMode2D(cam);
}

void IsometricCamera2D::end() const {
    EndMode2D();
}

void IsometricCamera2D::follow(int gx, int gy) {
    grid_x_ = gx;
    grid_y_ = gy;
}

void IsometricCamera2D::pan(int dx, int dy) {
    grid_x_ += dx;
    grid_y_ += dy;
}

void IsometricCamera2D::pan_to(int gx, int gy) {
    grid_x_ = gx;
    grid_y_ = gy;
}

void IsometricCamera2D::pan_by_delta(float dx, float dy) {
    // Convert screen pixel delta to grid delta
    // At zoom=1, one grid unit = TILE_WIDTH pixels in isometric space
    // But we need to convert screen delta to grid delta based on camera position
    float grid_delta_x = dx / (TILE_WIDTH / 2.0f) / zoom_;
    float grid_delta_y = dy / (TILE_HEIGHT / 2.0f) / zoom_;
    
    // In isometric: moving right on screen = moving in positive gx direction
    // moving down on screen = moving in positive gy direction
    grid_x_ += static_cast<int>(std::round(grid_delta_x + grid_delta_y) / 2.0f);
    grid_y_ += static_cast<int>(std::round(grid_delta_y - grid_delta_x) / 2.0f);
}

void IsometricCamera2D::update(float dt) {
    float dx = static_cast<float>(grid_x_) - static_cast<float>(visual_x_);
    float dy = static_cast<float>(grid_y_) - static_cast<float>(visual_y_);
    
    float move = follow_speed_ * dt;
    
    if (std::abs(dx) <= move) {
        visual_x_ = grid_x_;
    } else {
        visual_x_ += (dx > 0 ? 1 : -1) * static_cast<int>(std::floor(move));
    }
    
    if (std::abs(dy) <= move) {
        visual_y_ = grid_y_;
    } else {
        visual_y_ += (dy > 0 ? 1 : -1) * static_cast<int>(std::floor(move));
    }
}

void IsometricCamera2D::zoom_at(float factor, Vector2 screen_pos) {
    float old_zoom = zoom_;
    zoom_ *= factor;
    
    if (zoom_ < CAMERA_MIN_ZOOM) zoom_ = CAMERA_MIN_ZOOM;
    if (zoom_ > CAMERA_MAX_ZOOM) zoom_ = CAMERA_MAX_ZOOM;
    
    if (zoom_ == old_zoom) return;
    
    Vector2 rel_before = {
        (screen_pos.x - offset_.x) / old_zoom,
        (screen_pos.y - offset_.y) / old_zoom
    };
    
    Vector2 rel_after = {
        rel_before.x * zoom_,
        rel_before.y * zoom_
    };
    
    offset_.x += (rel_before.x - rel_after.x);
    offset_.y += (rel_before.y - rel_after.y);
}

void IsometricCamera2D::set_zoom(float z) {
    zoom_ = z;
    target_zoom_ = z;
    if (zoom_ < CAMERA_MIN_ZOOM) zoom_ = CAMERA_MIN_ZOOM;
    if (zoom_ > CAMERA_MAX_ZOOM) zoom_ = CAMERA_MAX_ZOOM;
}

void IsometricCamera2D::set_target_zoom(float z) {
    target_zoom_ = z;
    if (target_zoom_ < CAMERA_MIN_ZOOM) target_zoom_ = CAMERA_MIN_ZOOM;
    if (target_zoom_ > CAMERA_MAX_ZOOM) target_zoom_ = CAMERA_MAX_ZOOM;
}

void IsometricCamera2D::update_zoom(float dt) {
    if (std::abs(zoom_ - target_zoom_) < 0.001f) {
        zoom_ = target_zoom_;
        return;
    }
    
    float diff = target_zoom_ - zoom_;
    float step = zoom_speed_ * dt;
    
    if (std::abs(diff) <= step) {
        zoom_ = target_zoom_;
    } else {
        zoom_ += (diff > 0 ? 1 : -1) * step;
    }
}

float IsometricCamera2D::get_zoom() const {
    return zoom_;
}

int IsometricCamera2D::target_grid_x() const { return grid_x_; }
int IsometricCamera2D::target_grid_y() const { return grid_y_; }
int IsometricCamera2D::visual_grid_x() const { return visual_x_; }
int IsometricCamera2D::visual_grid_y() const { return visual_y_; }
float IsometricCamera2D::get_follow_speed() const { return follow_speed_; }
void IsometricCamera2D::set_follow_speed(float s) { follow_speed_ = s; }

void IsometricCamera2D::snap_to_target() {
    visual_x_ = grid_x_;
    visual_y_ = grid_y_;
}

} // namespace ns