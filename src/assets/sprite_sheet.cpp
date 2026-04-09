#include "sprite_sheet.h"
#include <iostream>

namespace ns {

void SpriteSheet::load(const std::string& path, int frame_width, int frame_height, int frames_x, int frames_y) {
    texture_ = LoadTexture(path.c_str());
    if (texture_.id == 0) {
        std::cerr << "[SpriteSheet] Failed to load: " << path << std::endl;
        return;
    }
    frame_width_ = frame_width;
    frame_height_ = frame_height;
    frames_x_ = frames_x;
    frames_y_ = frames_y;
}

Rectangle SpriteSheet::get_frame(int row, int col) const {
    return {
        static_cast<float>(col * frame_width_),
        static_cast<float>(row * frame_height_),
        static_cast<float>(frame_width_),
        static_cast<float>(frame_height_)
    };
}

Texture2D SpriteSheet::texture() const { return texture_; }
bool SpriteSheet::is_loaded() const { return texture_.id != 0; }

} // namespace ns
