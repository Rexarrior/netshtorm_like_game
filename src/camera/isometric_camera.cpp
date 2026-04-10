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
    // HALF_W = TILE_WIDTH/2 = 32, HALF_H = TILE_HEIGHT/2 = 16
    //
    // In isometric projection:
    //   screen_x = (gx - gy) * HALF_W,  screen_y = (gx + gy) * HALF_H
    //
    // Camera at (cam_gx, cam_gy) means grid origin is at that screen position.
    // For a point P at (gx, gy), its screen position relative to camera is:
    //   rel_x = (gx - gy - cam_gx + cam_gy) * HALF_W
    //   rel_y = (gx + gy - cam_gx - cam_gy) * HALF_H
    //
    // When camera moves by delta_cam = (delta_gx, delta_gy) in grid space,
    // the relative position of point P changes by:
    //   delta_rel_x = (-delta_gx + delta_gy) * HALF_W
    //   delta_rel_y = (-delta_gx - delta_gy) * HALF_H
    //
    // When user drags on screen, content appears to move opposite:
    //   content_move_x = -delta_rel_x,  content_move_y = -delta_rel_y
    //   So: dx = (delta_gx - delta_gy) * HALF_W,  dy = (delta_gx + delta_gy) * HALF_H
    //
    // Solving the system for camera movement:
    //   delta_gx = (dx/HALF_W + dy/HALF_H) / 2
    //   delta_gy = (dx/HALF_W - dy/HALF_H) / 2
    float HALF_W = TILE_WIDTH / 2.0f;
    float HALF_H = TILE_HEIGHT / 2.0f;
    
    float delta_gx = (dx / HALF_W + dy / HALF_H) / (2.0f * zoom_);
    float delta_gy = (dx / HALF_W - dy / HALF_H) / (2.0f * zoom_);
    
    grid_x_ += static_cast<int>(std::round(delta_gx));
    grid_y_ += static_cast<int>(std::round(delta_gy));
}

void IsometricCamera2D::update(float dt) {
    if (dt <= 0.0f) return;  // No movement if no time passed
    
    float dx = static_cast<float>(grid_x_) - static_cast<float>(visual_x_);
    float dy = static_cast<float>(grid_y_) - static_cast<float>(visual_y_);
    
    float move = follow_speed_ * dt;
    
    // Move at least 1 unit if there's significant distance and move < 1
    int move_x = 0, move_y = 0;
    if (std::abs(dx) > 0.001f) {
        if (std::abs(dx) <= move) {
            move_x = static_cast<int>(dx);
        } else {
            move_x = (dx > 0 ? 1 : -1) * std::max(1, static_cast<int>(std::floor(move)));
        }
    }
    if (std::abs(dy) > 0.001f) {
        if (std::abs(dy) <= move) {
            move_y = static_cast<int>(dy);
        } else {
            move_y = (dy > 0 ? 1 : -1) * std::max(1, static_cast<int>(std::floor(move)));
        }
    }
    
    visual_x_ += move_x;
    visual_y_ += move_y;
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