#pragma once

#include <raylib.h>
#include "input_actions.h"
#include <vector>

namespace ns {

class InputHandler {
public:
    void update();
    bool is_action_pressed(InputAction action) const;
    bool is_action_just_pressed(InputAction action) const;
    Vector2 get_mouse_position() const;
    Vector2 get_mouse_delta() const;  // Returns mouse movement delta during drag
    bool is_mouse_left_pressed() const;
    bool is_mouse_left_just_pressed() const;
    bool is_mouse_right_pressed() const;
    bool is_pan_drag_active() const;  // true when right-click or cmd+left-click drag
    float get_mouse_wheel_move() const;

private:
    std::vector<InputAction> pressed_actions_;
    std::vector<InputAction> just_pressed_actions_;
    Vector2 mouse_pos_{};
    Vector2 drag_start_pos_{};
    Vector2 mouse_delta_{};
    float wheel_move_ = 0.0f;
    bool mouse_left_was_down_ = false;
    bool mouse_right_was_down_ = false;
    bool pan_drag_active_ = false;
};

} // namespace ns
