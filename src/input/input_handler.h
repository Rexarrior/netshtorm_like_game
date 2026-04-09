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
    bool is_mouse_left_pressed() const;
    bool is_mouse_left_just_pressed() const;
    float get_mouse_wheel_move() const;

private:
    std::vector<InputAction> pressed_actions_;
    std::vector<InputAction> just_pressed_actions_;
    Vector2 mouse_pos_{};
    float wheel_move_ = 0.0f;
    bool mouse_left_was_down_ = false;
};

} // namespace ns
