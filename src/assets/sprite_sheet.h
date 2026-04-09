#pragma once

#include <raylib.h>
#include <string>

namespace ns {

class SpriteSheet {
public:
    void load(const std::string& path, int frame_width, int frame_height, int frames_x, int frames_y);
    Rectangle get_frame(int row, int col) const;
    Texture2D texture() const;
    bool is_loaded() const;
    int frame_width() const { return frame_width_; }
    int frame_height() const { return frame_height_; }

private:
    Texture2D texture_{};
    int frame_width_ = 0;
    int frame_height_ = 0;
    int frames_x_ = 0;
    int frames_y_ = 0;
};

} // namespace ns
